// Copyright 2018 (c) rust-themis developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! Secure Session service.
//!
//! **Secure Session** is a lightweight mechanism for securing any kind of network communication
//! (both private and public networks, including the Internet). It is protocol-agnostic and
//! operates on the 5th layer of the network OSI model (the session layer).
//!
//! Communication over Secure Session consists of two stages:
//!
//!   - negotiation stage (key agreement);
//!   - actual data exchange.
//!
//! During the negotiation stage, peers exchange their cryptographic material and authenticate
//! each other. After a successful mutual authentication, each peer derives a session-shared
//! secret and other auxiliary info from the session (session ID, initial vectors, etc.)
//!
//! During the data exchange stage, peers securely exchange data provided by higher layer
//! protocols.
//!
//! # Examples
//!
//! Secure Session usage is relatively involved so you can see a complete working example in the
//! documentation: [client] and [server].
//!
//! [client]: https://github.com/cossacklabs/themis/blob/master/docs/examples/rust/secure_session_echo_client.rs
//! [server]: https://github.com/cossacklabs/themis/blob/master/docs/examples/rust/secure_session_echo_server.rs
//!
//! To sum it up, you begin by implementing a [`SecureSessionTransport`]. You have to implement
//! at least the [`get_public_key_for_id`] method and may want to implement some others.
//! Then you acquire the asymmetric key pairs and distribute the public keys associated with
//! _peer IDs_ — arbitrary byte strings used to identify communicating Secure Sessions.
//! With that you can create an instance of [`SecureSession`] on both the client and the server.
//!
//! Next you go through _the negotiation stage_ using [`connect`] and [`negotiate`]
//! methods until the connection [`is_established`]. After that the Secure Sessions are ready
//! for _data exchange_ which is performed using [`send`] and [`receive`] methods.
//!
//! There is also an alternative buffer-oriented API. See [`SecureSession`] documentation to learn
//! more.
//!
//! [`SecureSessionTransport`]: trait.SecureSessionTransport.html
//! [`get_public_key_for_id`]: trait.SecureSessionTransport.html#tymethod.get_public_key_for_id
//! [`SecureSession`]: struct.SecureSession.html
//! [`connect`]: struct.SecureSession.html#method.connect
//! [`negotiate`]: struct.SecureSession.html#method.negotiate
//! [`is_established`]: struct.SecureSession.html#method.is_established
//! [`send`]: struct.SecureSession.html#method.send
//! [`receive`]: struct.SecureSession.html#method.receive

use std::error;
use std::fmt;
use std::os::raw::{c_int, c_void};
use std::panic;
use std::{ptr, result, slice};

use bindings::{
    secure_session_connect, secure_session_create, secure_session_destroy,
    secure_session_generate_connect_request, secure_session_get_remote_id,
    secure_session_is_established, secure_session_receive, secure_session_send, secure_session_t,
    secure_session_unwrap, secure_session_user_callbacks_t, secure_session_wrap, STATE_ESTABLISHED,
    STATE_IDLE, STATE_NEGOTIATING,
};

use crate::error::{themis_status_t, Error, ErrorKind, Result};
use crate::keys::{EcdsaPrivateKey, EcdsaPublicKey};
use crate::utils::into_raw_parts;

/// Secure Session context.
///
/// This is _the_ Secure Session object used for secure data exchange.
///
/// Secure Session only provides security services and doesn’t do actual network communication.
/// In fact, Secure Session is decoupled and independent from any networking implementation.
/// It is your responsibility to provide a network transport for Secure Session using the
/// [`SecureSessionTransport`] trait. There are two types of APIs available: callback API
/// and buffer-aware API. You can choose whatever API is more suitable for your application,
/// or you can even mix them when appropriate.
///
/// [`SecureSessionTransport`]: trait.SecureSessionTransport.html
///
/// # Callback API
///
/// With the callback API you delegate network communication to Secure Session. In order
/// to use it you have to implement the [`send_data`] and [`receive_data`] callbacks of
/// [`SecureSessionTransport`]. Then you use [`connect`] and [`negotiate`] methods
/// to negotiate and establish connection. After that [`send`] and [`receive`] methods can
/// be used for data exchange. Secure Session will synchronously call the provided transport
/// methods when necessary to perform network communication.
///
/// The documentation contains [an example of a server] using the callback API.
///
/// [an example of a server]: https://github.com/cossacklabs/themis/blob/master/docs/examples/rust/secure_session_echo_server.rs
/// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
/// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
/// [`connect`]: struct.SecureSession.html#method.connect
/// [`negotiate`]: struct.SecureSession.html#method.negotiate
/// [`send`]: struct.SecureSession.html#method.send
/// [`receive`]: struct.SecureSession.html#method.receive
///
/// # Buffer-aware API
///
/// With the buffer-aware API you are responsible for transporting Secure Session messages
/// between peers. Secure Session does not use `send_data` and `receive_data` callbacks in this
/// mode. Instead the [`connect_request`] and [`negotiate_reply`] methods return and receive
/// data buffers that have to be exchanged between peers via some external transport (e.g., TLS).
/// Similarly, [`wrap`] and [`unwrap`] methods are used to encrypt and decrypt data exchange
/// messages after the connection has been negotiated. They too accept plaintext messages and
/// return encrypted containers or vice versa.
///
/// The documentation contains [an example of a client] using the buffer-aware API.
///
/// [an example of a client]: https://github.com/cossacklabs/themis/blob/master/docs/examples/rust/secure_session_echo_client.rs
/// [`connect_request`]: struct.SecureSession.html#method.connect_request
/// [`negotiate_reply`]: struct.SecureSession.html#method.negotiate_reply
/// [`wrap`]: struct.SecureSession.html#method.wrap
/// [`unwrap`]: struct.SecureSession.html#method.unwrap
pub struct SecureSession {
    session: *mut secure_session_t,

    // Secure Session instance keeps around pointers to callbacks and contexts internally.
    // This box is important to have the structure address pinned in memory. It looks "dead"
    // to Rust code, but we actually need to keep it alive for the C code.
    #[allow(dead_code)]
    context: Box<SecureSessionContext>,
}

struct SecureSessionContext {
    callbacks: secure_session_user_callbacks_t,
    transport: Box<dyn SecureSessionTransport>,
    last_error: Option<TransportError>,
}

// It safe to move secure_session_t to another thread, it does not depend on any thread-local
// state. However, it needs external synchronization for safe concurrent usage (hence no Sync).
unsafe impl Send for SecureSession {}

// secure_session_user_callbacks_t in SecureSessionContext holds a pointer to itself and thus
// it is !Send by default. However, we keep SecureSessionContext in a box, pinned in memory,
// and that pointer is the only extra reference.
unsafe impl Send for SecureSessionContext {}

/// Transport delegate for Secure Session.
///
/// This is an interface you need to provide for Secure Session operation.
///
/// The only required method is [`get_public_key_for_id`]. It is required for public key
/// authentication. Other methods are optional, you can use Secure Session without them,
/// but some functionality may be unavailable.
///
/// [`get_public_key_for_id`]: trait.SecureSessionTransport.html#tymethod.get_public_key_for_id
#[allow(unused_variables)]
pub trait SecureSessionTransport: Send {
    /// Get a public key corresponding to a remote peer ID.
    ///
    /// Return `None` if you are unable to locate a public key corresponding to the provided ID.
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey>;

    /// Send the provided data to the peer, return the number of bytes transferred.
    ///
    /// This method will be called when Secure Session needs to send some data to its peer.
    ///
    /// You need to send the entire message to the peer. It is your responsibility to perform
    /// any necessary framing, splitting, and retrying on the transport layer. Returning anything
    /// other than `Ok(data.len())` from this method is considered a transport failure.
    ///
    /// See also [`TransportError`] on how to handle transport layer failures.
    ///
    /// This method is used by [the callback API] ([`connect`], [`negotiate`], [`send`]).
    /// You need to implement it in order to use this API. You may ignore it if you only use
    /// [the buffer-aware API].
    ///
    /// [`TransportError`]: struct.TransportError.html
    /// [the callback API]: struct.SecureSession.html#callback-api
    /// [the buffer-aware API]: struct.SecureSession.html#buffer-aware-api
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`send`]: struct.SecureSession.html#method.send
    fn send_data(&mut self, data: &[u8]) -> result::Result<usize, TransportError> {
        Err(TransportError::unspecified())
    }

    /// Receive data from the peer into the provided buffer, return the number of bytes written.
    ///
    /// This method will be called when Secure Session expects to receive some data from its peer.
    /// The length of the buffer indicates the maximum amount of data expected.
    ///
    /// You need to store an entire message as sent by the peer into the buffer. It is your
    /// responsibility to decode any framing, reconstruct the message from split parts, and retry
    /// receiving on the transport layer until you have what appears to be a whole message.
    ///
    /// See also [`TransportError`] on how to handle transport layer failures.
    ///
    /// This method is used by [the callback API] ([`negotiate`], [`receive`]).
    /// You need to implement it in order to use this API. You may ignore it if you only use
    /// [the buffer-aware API].
    ///
    /// [`TransportError`]: struct.TransportError.html
    /// [the callback API]: struct.SecureSession.html#callback-api
    /// [the buffer-aware API]: struct.SecureSession.html#buffer-aware-api
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`receive`]: struct.SecureSession.html#method.receive
    fn receive_data(&mut self, data: &mut [u8]) -> result::Result<usize, TransportError> {
        Err(TransportError::unspecified())
    }

    /// Notification about the connection state of the Secure Session.
    ///
    /// This method is truly optional and has no effect on Secure Session operation.
    fn state_changed(&mut self, state: SecureSessionState) {}
}

/// Transport layer error.
///
/// This is a type representing failure in transport layer of [`SecureSessionTransport`],
/// namely its [`send_data`] and [`receive_data`] methods.
///
/// [`SecureSessionTransport`]: trait.SecureSessionTransport.html
/// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
/// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
///
/// # Examples
///
/// `TransportError` can conveniently wrap any other error using `?` operator.
/// You can also explicitly construct an error with a descriptive string.
///
/// ```no_run
/// use std::io::{Read, Write};
/// use std::net::TcpStream;
///
/// use themis::secure_session::{SecureSessionTransport, TransportError};
/// # use themis::keys::EcdsaPublicKey;
///
/// struct SocketTransport {
///     socket: TcpStream,
/// }
///
/// impl SecureSessionTransport for SocketTransport {
///     fn send_data(&mut self, data: &[u8]) -> Result<usize, TransportError> {
///         if data.len() >= 256 {
///             return Err(TransportError::new(format!("too long data: {} bytes", data.len())));
///         }
///
///         let len_buffer = [data.len() as u8];
///         self.socket.write_all(&len_buffer)?;
///
///         self.socket.write_all(data)?;
///         Ok(data.len())
///     }
///
///     fn receive_data(&mut self, data: &mut [u8]) -> Result<usize, TransportError> {
///         let mut len_buffer = [0];
///         self.socket.read_exact(&mut len_buffer)?;
///
///         let len = len_buffer[0] as usize;
///         if data.len() < len {
///             return Err(TransportError::new("buffer too short"));
///         }
///
///         self.socket.read_exact(&mut data[0..len])?;
///         Ok(len)
///     }
///
///     // Other methods omitted
/// #
/// #   fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
/// #       None
/// #   }
/// }
/// ```
pub struct TransportError {
    inner: TransportErrorInner,
}

enum TransportErrorInner {
    Unspecified,
    Simple(String),
    Custom(Box<dyn error::Error + Send + Sync>),
}

impl fmt::Display for TransportError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &self.inner {
            TransportErrorInner::Unspecified => write!(f, "Secure Session transport failed"),
            TransportErrorInner::Simple(s) => write!(f, "Secure Session transport failed: {s}"),
            TransportErrorInner::Custom(e) => write!(f, "Secure Session transport failed: {e}"),
        }
    }
}

impl fmt::Debug for TransportError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &self.inner {
            TransportErrorInner::Unspecified => write!(f, "TransportError::Unspecified"),
            TransportErrorInner::Simple(s) => write!(f, "TransportError::Simple({s:?})"),
            TransportErrorInner::Custom(e) => write!(f, "TransportError::Custom({e:?})"),
        }
    }
}

impl<T> From<T> for TransportError
where
    T: error::Error + Send + Sync + 'static,
{
    fn from(error: T) -> Self {
        TransportError {
            inner: TransportErrorInner::Custom(Box::new(error)),
        }
    }
}

impl TransportError {
    /// Returns a new error with a human-readable description.
    pub fn new(description: impl Into<String>) -> TransportError {
        TransportError {
            inner: TransportErrorInner::Simple(description.into()),
        }
    }

    /// Returns an unspecified error.
    pub fn unspecified() -> TransportError {
        TransportError {
            inner: TransportErrorInner::Unspecified,
        }
    }
}

/// State of Secure Session connection.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SecureSessionState {
    /// Newly created sessions start in this state.
    Idle,
    /// Connection establishment in progress.
    Negotiating,
    /// Connection has been established, data exchange may commence.
    Established,
}

impl SecureSessionState {
    fn from_int(state: c_int) -> Option<Self> {
        match state as u32 {
            STATE_IDLE => Some(SecureSessionState::Idle),
            STATE_NEGOTIATING => Some(SecureSessionState::Negotiating),
            STATE_ESTABLISHED => Some(SecureSessionState::Established),
            _ => None,
        }
    }
}

impl SecureSession {
    /// Creates a new Secure Session.
    ///
    /// ID is an arbitrary non-empty byte sequence used to identify this peer.
    pub fn new(
        id: impl AsRef<[u8]>,
        key: &EcdsaPrivateKey,
        transport: impl SecureSessionTransport + 'static,
    ) -> Result<Self> {
        // TODO: human-readable detailed descriptions for errors
        // It would be nice to tell the user what exactly is wrong with parameters.
        if id.as_ref().is_empty() {
            return Err(Error::with_kind(ErrorKind::InvalidParameter));
        }

        let (id_ptr, id_len) = into_raw_parts(id.as_ref());
        let (key_ptr, key_len) = into_raw_parts(key.as_ref());

        let mut context = Box::new(SecureSessionContext {
            callbacks: secure_session_user_callbacks_t {
                send_data: Some(send_data),
                receive_data: Some(receive_data),
                state_changed: Some(state_changed),
                get_public_key_for_id: Some(get_public_key_for_id),
                user_data: std::ptr::null_mut(),
            },
            transport: Box::new(transport),
            last_error: None,
        });
        context.callbacks.user_data = context_as_user_data(&context);

        let session = unsafe {
            secure_session_create(
                id_ptr as *const c_void,
                id_len,
                key_ptr as *const c_void,
                key_len,
                &context.callbacks,
            )
        };

        if session.is_null() {
            // This is most likely an allocation error but we have no way to know.
            return Err(Error::with_kind(ErrorKind::NoMemory));
        }

        Ok(Self { session, context })
    }

    /// Returns `true` if this Secure Session may be used for data transfer.
    pub fn is_established(&self) -> bool {
        unsafe { secure_session_is_established(self.session) }
    }

    // TODO: abstract out the 'check-allocate-leap' pattern
    //
    // This is really common here to call a C function to get a size of the buffer, then allocate
    // memory, then call the function again to do actual work, then fix the length of the vector.
    // It would be nice to have this abstracted out so that we don't have to repeat ourselves.

    /// Returns ID of the remote peer.
    ///
    /// Returns `None` if the connection has not been established yet and there is no peer.
    pub fn remote_peer_id(&self) -> Result<Option<Vec<u8>>> {
        let mut id = Vec::new();
        let mut id_len = 0;

        unsafe {
            let status = secure_session_get_remote_id(self.session, ptr::null_mut(), &mut id_len);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        id.reserve(id_len);

        unsafe {
            let status = secure_session_get_remote_id(self.session, id.as_mut_ptr(), &mut id_len);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(id_len <= id.capacity());
            id.set_len(id_len);
        }

        Ok(if id.is_empty() { None } else { Some(id) })
    }

    /// Initiates connection to the remote peer.
    ///
    /// This is the first method to call for the client, it sends a connection request to the
    /// server. Afterwards call [`negotiate`] until the connection is established.
    /// That is, until the [`state_changed`] callback of your `SecureSessionTransport` tells you
    /// that the connection is `Established`, or until [`is_established`] on this Secure Session
    /// returns `true`.
    ///
    /// This method is a part of callback API and requires [`send_data`] method of
    /// `SecureSessionTransport` to be implemented.
    ///
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`state_changed`]: trait.SecureSessionTransport.html#method.state_changed
    /// [`is_established`]: struct.SecureSession.html#method.is_established
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    pub fn connect(&mut self) -> Result<()> {
        unsafe {
            let status = secure_session_connect(self.session);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
        }
        Ok(())
    }

    /// Continues connection negotiation.
    ///
    /// This method performs one step of connection negotiation. This is the first method to call
    /// for the server, the client calls it after the initial [`connect`]. Both peers shall then
    /// repeatedly call this method until the connection is established (see [`connect`] for
    /// details).
    ///
    /// This method is a part of callback API and requires [`send_data`] and [`receive_data`]
    /// methods of `SecureSessionTransport` to be implemented.
    ///
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    /// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
    pub fn negotiate(&mut self) -> Result<()> {
        unsafe {
            let result = secure_session_receive(self.session, ptr::null_mut(), 0);
            if result == TRANSPORT_FAILURE {
                let error = self.context.last_error.take().expect("missing error");
                return Err(Error::from_transport_error(error));
            }
            if result == TRANSPORT_OVERFLOW {
                return Err(Error::with_kind(ErrorKind::BufferTooSmall));
            }
            if result == TRANSPORT_PANIC {
                return Err(Error::from_transport_error(TransportError::unspecified()));
            }
            let error = Error::from_session_status(result as themis_status_t);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
        }

        Ok(())
    }

    // TODO: make Themis improve the error reporting for send and receive
    //
    // Themis sends messages in full. Partial transfer is considered an error. In case of an
    // error the error code is returned in-band and cannot be distinguished from a successful
    // return of message length. This is the best we can do at the moment.
    //
    // Furthermore, Themis expects the send callback to send the whole message so it is kinda
    // pointless to return the amount of bytes send. The receive callback returns accurate number
    // of bytes, but I do not really like the Rust interface this implies. It could be made better.
    const THEMIX_MAX_ERROR: isize = 21;

    /// Sends a message to the remote peer.
    ///
    /// This method will fail if a secure connection has not been established yet. See [`connect`]
    /// and [`negotiate`] methods for establishing a connection.
    ///
    /// This method is a part of callback API and requires [`send_data`] method of
    /// `SecureSessionTransport` to be implemented.
    ///
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    pub fn send(&mut self, message: impl AsRef<[u8]>) -> Result<()> {
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        unsafe {
            let length =
                secure_session_send(self.session, message_ptr as *const c_void, message_len);
            if length == TRANSPORT_FAILURE {
                let error = self.context.last_error.take().expect("missing error");
                return Err(Error::from_transport_error(error));
            }
            if length == TRANSPORT_OVERFLOW {
                return Err(Error::with_kind(ErrorKind::BufferTooSmall));
            }
            if length == TRANSPORT_PANIC {
                return Err(Error::from_transport_error(TransportError::unspecified()));
            }
            if length <= Self::THEMIX_MAX_ERROR {
                return Err(Error::from_session_status(length as themis_status_t));
            }
        }

        Ok(())
    }

    /// Receives a message from the remote peer.
    ///
    /// Maximum length of the message is specified by the parameter.
    ///
    /// This method will fail if a secure connection has not been established yet. See [`connect`]
    /// and [`negotiate`] methods for establishing a connection.
    ///
    /// This method is a part of callback API and requires [`receive_data`] method of
    /// `SecureSessionTransport` to be implemented.
    ///
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
    pub fn receive(&mut self, max_len: usize) -> Result<Vec<u8>> {
        let mut message = Vec::with_capacity(max_len);

        unsafe {
            let length = secure_session_receive(
                self.session,
                message.as_mut_ptr() as *mut c_void,
                message.capacity(),
            );
            if length == TRANSPORT_FAILURE {
                let error = self.context.last_error.take().expect("missing error");
                return Err(Error::from_transport_error(error));
            }
            if length == TRANSPORT_OVERFLOW {
                return Err(Error::with_kind(ErrorKind::BufferTooSmall));
            }
            if length == TRANSPORT_PANIC {
                return Err(Error::from_transport_error(TransportError::unspecified()));
            }
            if length <= Self::THEMIX_MAX_ERROR {
                return Err(Error::from_session_status(length as themis_status_t));
            }
            debug_assert!(length as usize <= message.capacity());
            message.set_len(length as usize);
        }

        Ok(message)
    }

    /// Initiates connection to the remote peer, returns connection message.
    ///
    /// This is the first method to call for the client, it returns you a message that you must
    /// transfer to the server. The server then shall give the message to its [`negotiate_reply`]
    /// method which returns a reply that must be transferred back to this Secure Session and
    /// passed to its [`negotiate_reply`] method. Continue passing these message around until the
    /// connection is established. That is, until the [`state_changed`] callback of your
    /// `SecureSessionTransport` tells you that the connection is `Established`, or until
    /// [`is_established`] on this Secure Session returns `true`, or until [`negotiate_reply`]
    /// returns an empty message.
    ///
    /// [`negotiate_reply`]: struct.SecureSession.html#method.negotiate_reply
    /// [`state_changed`]: trait.SecureSessionTransport.html#method.state_changed
    /// [`is_established`]: struct.SecureSession.html#method.is_established
    pub fn connect_request(&mut self) -> Result<Vec<u8>> {
        let mut output = Vec::new();
        let mut output_len = 0;

        unsafe {
            let status = secure_session_generate_connect_request(
                self.session,
                ptr::null_mut(),
                &mut output_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        output.reserve(output_len);

        unsafe {
            let status = secure_session_generate_connect_request(
                self.session,
                output.as_mut_ptr() as *mut c_void,
                &mut output_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(output_len <= output.capacity());
            output.set_len(output_len);
        }

        Ok(output)
    }

    /// Continues connection negotiation with given message.
    ///
    /// This method performs one step of connection negotiation. The server should call this
    /// method first with a message received from client’s [`connect_request`].
    /// Its result is another negotiation message that should be transferred to the client.
    /// The client then calls this method on a message and forwards the resulting message
    /// to the server. If the returned message is empty then negotiation is complete and
    /// the Secure Session is ready to be used.
    ///
    /// [`negotiate_reply`]: struct.SecureSession.html#method.negotiate_reply
    /// [`connect_request`]: struct.SecureSession.html#method.connect_request
    pub fn negotiate_reply(&mut self, wrapped: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (wrapped_ptr, wrapped_len) = into_raw_parts(wrapped.as_ref());

        let mut message = Vec::new();
        let mut message_len = 0;

        unsafe {
            let status = secure_session_unwrap(
                self.session,
                wrapped_ptr as *const c_void,
                wrapped_len,
                ptr::null_mut(),
                &mut message_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() == ErrorKind::Success {
                return Ok(message);
            }
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        message.reserve(message_len);

        unsafe {
            let status = secure_session_unwrap(
                self.session,
                wrapped_ptr as *const c_void,
                wrapped_len,
                message.as_mut_ptr() as *mut c_void,
                &mut message_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::SessionSendOutputToPeer {
                assert_ne!(error.kind(), ErrorKind::Success);
                return Err(error);
            }
            debug_assert!(message_len <= message.capacity());
            message.set_len(message_len);
        }

        Ok(message)
    }

    /// Encrypts a message and returns it.
    ///
    /// The message can be transferred to the remote peer and decrypted there with [`unwrap`].
    ///
    /// Messages are independent and can be exchanged out of order. You can encrypt multiple
    /// messages then decrypt them in any order or don’t decrypt some of them at all.
    ///
    /// This method will fail if a secure connection has not been established yet.
    /// See [`connect_request`] and [`negotiate_reply`] methods for establishing a connection.
    ///
    /// [`unwrap`]: struct.SecureSession.html#method.unwrap
    /// [`connect_request`]: struct.SecureSession.html#method.connect_request
    /// [`negotiate_reply`]: struct.SecureSession.html#method.negotiate_reply
    pub fn wrap(&mut self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        let mut wrapped = Vec::new();
        let mut wrapped_len = 0;

        unsafe {
            let status = secure_session_wrap(
                self.session,
                message_ptr as *const c_void,
                message_len,
                ptr::null_mut(),
                &mut wrapped_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        wrapped.reserve(wrapped_len);

        unsafe {
            let status = secure_session_wrap(
                self.session,
                message_ptr as *const c_void,
                message_len,
                wrapped.as_mut_ptr() as *mut c_void,
                &mut wrapped_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(wrapped_len <= wrapped.capacity());
            wrapped.set_len(wrapped_len);
        }

        Ok(wrapped)
    }

    /// Decrypts a message and returns it.
    ///
    /// Decrypts a message previously [`wrapped`] by the remote peer.
    ///
    /// This method will fail if a secure connection has not been established yet.
    /// See [`connect_request`] and [`negotiate_reply`] methods for establishing a connection.
    ///
    /// [`wrapped`]: struct.SecureSession.html#method.wrap
    /// [`connect_request`]: struct.SecureSession.html#method.connect_request
    /// [`negotiate_reply`]: struct.SecureSession.html#method.negotiate_reply
    pub fn unwrap(&mut self, wrapped: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (wrapped_ptr, wrapped_len) = into_raw_parts(wrapped.as_ref());

        let mut message = Vec::new();
        let mut message_len = 0;

        unsafe {
            let status = secure_session_unwrap(
                self.session,
                wrapped_ptr as *const c_void,
                wrapped_len,
                ptr::null_mut(),
                &mut message_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        message.reserve(message_len);

        unsafe {
            let status = secure_session_unwrap(
                self.session,
                wrapped_ptr as *const c_void,
                wrapped_len,
                message.as_mut_ptr() as *mut c_void,
                &mut message_len,
            );
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(message_len <= message.capacity());
            message.set_len(message_len);
        }

        Ok(message)
    }
}

impl fmt::Debug for SecureSession {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        f.debug_struct("SecureSession")
            .field("session", &self.session)
            .finish()
    }
}

// Secure Session C callback interface. Most of these functions are unsafe for general use.
// Pay attention when and how you call them.

// It is important for the context to be boxed, hence this lint suppression.
#[allow(clippy::borrowed_box)]
fn context_as_user_data(context: &Box<SecureSessionContext>) -> *mut c_void {
    (context.as_ref() as *const SecureSessionContext) as *mut c_void
}

unsafe fn user_data_as_context<'a>(ptr: *mut c_void) -> &'a mut SecureSessionContext {
    &mut *(ptr as *mut SecureSessionContext)
}

const TRANSPORT_FAILURE: isize = -1;
const TRANSPORT_OVERFLOW: isize = -2;
const TRANSPORT_PANIC: isize = -3;

unsafe extern "C" fn send_data(
    data_ptr: *const u8,
    data_len: usize,
    user_data: *mut c_void,
) -> isize {
    let result = panic::catch_unwind(|| {
        let data = byte_slice_from_ptr(data_ptr, data_len);
        let context = user_data_as_context(user_data);

        match context.transport.send_data(data) {
            Ok(sent_bytes) => as_isize(sent_bytes).unwrap_or(TRANSPORT_OVERFLOW),
            Err(error) => {
                context.last_error = Some(error);
                TRANSPORT_FAILURE
            }
        }
    });
    result.unwrap_or(TRANSPORT_PANIC)
}

unsafe extern "C" fn receive_data(
    data_ptr: *mut u8,
    data_len: usize,
    user_data: *mut c_void,
) -> isize {
    let result = panic::catch_unwind(|| {
        let data = byte_slice_from_ptr_mut(data_ptr, data_len);
        let context = user_data_as_context(user_data);

        match context.transport.receive_data(data) {
            Ok(received_bytes) => as_isize(received_bytes).unwrap_or(TRANSPORT_OVERFLOW),
            Err(error) => {
                context.last_error = Some(error);
                TRANSPORT_FAILURE
            }
        }
    });
    result.unwrap_or(TRANSPORT_PANIC)
}

unsafe extern "C" fn state_changed(event: c_int, user_data: *mut c_void) {
    let _ = panic::catch_unwind(|| {
        let transport = &mut user_data_as_context(user_data).transport;

        if let Some(state) = SecureSessionState::from_int(event) {
            transport.state_changed(state);
        }
    });
}

const GET_PUBLIC_KEY_SUCCESS: c_int = 0;
const GET_PUBLIC_KEY_FAILURE: c_int = -1;

unsafe extern "C" fn get_public_key_for_id(
    id_ptr: *const c_void,
    id_len: usize,
    key_ptr: *mut c_void,
    key_len: usize,
    user_data: *mut c_void,
) -> c_int {
    let result = panic::catch_unwind(|| {
        let id = byte_slice_from_ptr(id_ptr as *const u8, id_len);
        let key_out = byte_slice_from_ptr_mut(key_ptr as *mut u8, key_len);
        let transport = &mut user_data_as_context(user_data).transport;

        if let Some(key) = transport.get_public_key_for_id(id) {
            let key = key.as_ref();
            if key_out.len() >= key.len() {
                key_out[0..key.len()].copy_from_slice(key);
                return GET_PUBLIC_KEY_SUCCESS;
            }
        }

        GET_PUBLIC_KEY_FAILURE
    });
    result.unwrap_or(GET_PUBLIC_KEY_FAILURE)
}

#[doc(hidden)]
impl Drop for SecureSession {
    fn drop(&mut self) {
        unsafe {
            let status = secure_session_destroy(self.session);
            let error = Error::from_session_status(status);
            if (cfg!(debug) || cfg!(test)) && error.kind() != ErrorKind::Success {
                panic!("secure_session_destroy() failed: {}", error);
            }
        }
    }
}

fn as_isize(n: usize) -> Option<isize> {
    if n <= isize::max_value() as usize {
        Some(n as isize)
    } else {
        None
    }
}

// These functions are technically unsafe. You must trust the C code to give you correct pointers
// and lengths. Note that empty Rust slices must *not* be constructed from a null raw pointer,
// they should use a special value instead. This is important for some LLVM magic.

unsafe fn byte_slice_from_ptr<'a>(ptr: *const u8, len: usize) -> &'a [u8] {
    slice::from_raw_parts(escape_null_ptr(ptr as *mut u8), len)
}

unsafe fn byte_slice_from_ptr_mut<'a>(ptr: *mut u8, len: usize) -> &'a mut [u8] {
    slice::from_raw_parts_mut(escape_null_ptr(ptr), len)
}

fn escape_null_ptr<T>(ptr: *mut T) -> *mut T {
    if ptr.is_null() {
        ptr::NonNull::dangling().as_ptr()
    } else {
        ptr
    }
}
