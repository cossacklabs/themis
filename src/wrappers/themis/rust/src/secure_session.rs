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
//! (both private and public networks, including the Internet).

use std::os::raw::{c_int, c_void};
use std::{ptr, result, slice};

use bindings::{
    secure_session_connect, secure_session_create, secure_session_destroy,
    secure_session_generate_connect_request, secure_session_get_remote_id,
    secure_session_is_established, secure_session_receive, secure_session_send, secure_session_t,
    secure_session_unwrap, secure_session_user_callbacks_t, secure_session_wrap, STATE_ESTABLISHED,
    STATE_IDLE, STATE_NEGOTIATING,
};
use error::{themis_status_t, Error, ErrorKind, Result};
use keys::{EcdsaPublicKey, EcdsaSecretKey};
use utils::into_raw_parts;

/// Secure Session context.
pub struct SecureSession<T> {
    session_ctx: *mut secure_session_t,
    _delegate: Box<SecureSessionDelegate<T>>,
}

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
pub trait SecureSessionTransport {
    // TODO: consider send/receive use std::io::Error for errors (or a custom type)

    /// Send the provided data to the peer, return the number of bytes transferred.
    ///
    /// This callback will be called when Secure Session needs to send some data to its peer.
    /// The whole message is expected to be transferred so returning anything other than
    /// `Ok(data.len())` is considered an error.
    ///
    /// This method is used by the transport API ([`connect`], [`negotiate_transport`], [`send`]).
    /// You need to implement it in order to use this API.
    ///
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`negotiate_transport`]: struct.SecureSession.html#method.negotiate_transport
    /// [`send`]: struct.SecureSession.html#method.send
    fn send_data(&mut self, data: &[u8]) -> result::Result<usize, ()> {
        Err(())
    }

    /// Receive some data from the peer into the provided buffer, return the number of bytes.
    ///
    /// This callback will be called when Secure Session expects to receive some data. The length
    /// of the buffer indicates the maximum amount of data expected. Put the received data into
    /// the provided buffer and return the number of bytes that you used.
    ///
    /// This method is used by the transport API ([`negotiate_transport`], [`receive`]).
    /// You need to implement it in order to use this API.
    ///
    /// [`negotiate_transport`]: struct.SecureSession.html#method.negotiate_transport
    /// [`receive`]: struct.SecureSession.html#method.receive
    fn receive_data(&mut self, data: &mut [u8]) -> result::Result<usize, ()> {
        Err(())
    }

    /// Notification about connection state of Secure Session.
    ///
    /// This method is truly optional and has no effect on Secure Session operation.
    fn state_changed(&mut self, state: SecureSessionState) {}

    /// Get a public key corresponding to a peer ID.
    ///
    /// Return `None` if you are unable to find a corresponding public key.
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey>;
}

// We keep this struct in a box so that it has fixed address. Themis does *not* copy
// the callback struct into session context, it keeps a pointer to it. The callback
// structure itself also stores a `user_data` pointer to itself, so it's important
// to have this structure pinned in memory.
struct SecureSessionDelegate<T> {
    callbacks: secure_session_user_callbacks_t,
    transport: T,
}

/// State of Secure Session connection.
#[derive(PartialEq, Eq)]
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

impl<T> SecureSession<T>
where
    T: SecureSessionTransport,
{
    // TODO: introduce a builder

    /// Creates a new Secure Session.
    ///
    /// ID is an arbitrary byte sequence used to identify this peer.
    ///
    /// Secure Session supports only ECDSA keys.
    pub fn with_transport<I>(id: I, key: &EcdsaSecretKey, transport: T) -> Result<Self>
    where
        I: AsRef<[u8]>,
    {
        let (id_ptr, id_len) = into_raw_parts(id.as_ref());
        let (key_ptr, key_len) = into_raw_parts(key.as_ref());
        let delegate = SecureSessionDelegate::new(transport);

        let user_callbacks = delegate.user_callbacks();
        let session_ctx = unsafe {
            secure_session_create(
                id_ptr as *const c_void,
                id_len,
                key_ptr as *const c_void,
                key_len,
                user_callbacks,
            )
        };

        if session_ctx.is_null() {
            // Technically, this may be an allocation error but we have no way to know so just
            // assume that the user messed up and provided invalid keys (which is more likely).
            return Err(Error::with_kind(ErrorKind::InvalidParameter));
        }

        Ok(Self {
            session_ctx,
            _delegate: delegate,
        })
    }

    /// Returns `true` if this Secure Session may be used for data transfer.
    pub fn is_established(&self) -> bool {
        unsafe { secure_session_is_established(self.session_ctx) }
    }

    // TODO: abstract out the 'check-allocate-leap' pattern
    //
    // This is really common here to call a C function to get a size of the buffer, then allocate
    // memory, then call the function again to do actual work, then fix the length of the vector.
    // It would be nice to have this abstracted out so that we don't have to repeat ourselves.

    /// Returns ID of the remote peer.
    ///
    /// This method will return an error if the connection has not been established yet.
    pub fn get_remote_id(&self) -> Result<Vec<u8>> {
        let mut id = Vec::new();
        let mut id_len = 0;

        unsafe {
            let status =
                secure_session_get_remote_id(self.session_ctx, ptr::null_mut(), &mut id_len);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        id.reserve(id_len);

        unsafe {
            let status =
                secure_session_get_remote_id(self.session_ctx, id.as_mut_ptr(), &mut id_len);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(id_len <= id.capacity());
            id.set_len(id_len);
        }

        Ok(id)
    }

    /// Initiates connection to the remote peer.
    ///
    /// This is the first method to call. It uses transport callbacks to send the resulting
    /// connection request to the peer. Afterwards call [`negotiate_transport`] until the
    /// connection is established. That is, until the [`state_changed`] callback of your
    /// `SecureSessionTransport` tells you that the connection is `Established`, or until
    /// [`is_established`] on this Secure Session returns `true`.
    ///
    /// This method is a part of transport API and requires [`send_data`] method of
    /// `SecureSessionTransport`.
    ///
    /// [`negotiate_transport`]: struct.SecureSession.html#method.negotiate_transport
    /// [`state_changed`]: trait.SecureSessionTransport.html#method.state_changed
    /// [`is_established`]: struct.SecureSession.html#method.is_established
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    pub fn connect(&mut self) -> Result<()> {
        unsafe {
            let status = secure_session_connect(self.session_ctx);
            let error = Error::from_session_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
        }
        Ok(())
    }

    /// Initiates connection to the remote peer, returns connection message.
    ///
    /// This is the first method to call. It returns you a message that you must somehow
    /// transfer to the remote peer and give it to its [`negotiate`] method. This results in
    /// another message which must be transferred back to this Secure Session and passed to
    /// its [`negotiate`] method. Continue passing these message around until the connection
    /// is established. That is, until the [`state_changed`] callback of your
    /// `SecureSessionTransport` tells you that the connection is `Established`, or until
    /// [`is_established`] on this Secure Session returns `true`, or until [`negotiate`]
    /// returns an empty message.
    ///
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`state_changed`]: trait.SecureSessionTransport.html#method.state_changed
    /// [`is_established`]: struct.SecureSession.html#method.is_established
    pub fn generate_connect_request(&mut self) -> Result<Vec<u8>> {
        let mut output = Vec::new();
        let mut output_len = 0;

        unsafe {
            let status = secure_session_generate_connect_request(
                self.session_ctx,
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
                self.session_ctx,
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

    /// Wraps a message and returns it.
    ///
    /// The message can be transferred to the remote peer and unwrapped there with [`unwrap`].
    ///
    /// Wrapped message are independent and can be exchanged out of order. You can wrap multiple
    /// messages then unwrap them in any order or don't unwrap some of them at all.
    ///
    /// This method will fail if a secure connection has not been established yet.
    ///
    /// [`unwrap`]: struct.SecureSession.html#method.unwrap
    pub fn wrap<M: AsRef<[u8]>>(&mut self, message: M) -> Result<Vec<u8>> {
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        let mut wrapped = Vec::new();
        let mut wrapped_len = 0;

        unsafe {
            let status = secure_session_wrap(
                self.session_ctx,
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
                self.session_ctx,
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

    /// Unwraps a message and returns it.
    ///
    /// Unwraps a message previously [wrapped] by the remote peer.
    ///
    /// This method will fail if a secure connection has not been established yet.
    ///
    /// [wrapped]: struct.SecureSession.html#method.wrap
    pub fn unwrap<M: AsRef<[u8]>>(&mut self, wrapped: M) -> Result<Vec<u8>> {
        let (wrapped_ptr, wrapped_len) = into_raw_parts(wrapped.as_ref());

        let mut message = Vec::new();
        let mut message_len = 0;

        unsafe {
            let status = secure_session_unwrap(
                self.session_ctx,
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
                self.session_ctx,
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

    /// Continues connection negotiation with given message.
    ///
    /// This method performs one step of connection negotiation. The server should call this
    /// method first with a message received from client's [`generate_connect_request`].
    /// Its result is another negotiation message that should be transferred to the client.
    /// The client then calls this method on a message and forwards the resulting message
    /// to the server. If the returned message is empty then negotiation is complete and
    /// the Secure Session is ready to be used.
    ///
    /// [`negotiate`]: struct.SecureSession.html#method.negotiate
    /// [`generate_connect_request`]: struct.SecureSession.html#method.generate_connect_request
    pub fn negotiate<M: AsRef<[u8]>>(&mut self, wrapped: M) -> Result<Vec<u8>> {
        let (wrapped_ptr, wrapped_len) = into_raw_parts(wrapped.as_ref());

        let mut message = Vec::new();
        let mut message_len = 0;

        unsafe {
            let status = secure_session_unwrap(
                self.session_ctx,
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
                self.session_ctx,
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

    // TODO: make Themis improve the error reporting for send and receive
    //
    // Themis sends messages in full. Partial transfer is considered an error. In case of an
    // error the error code is returned in-band and cannot be distinguished from a successful
    // return of message length. This is the best we can do at the moment.
    //
    // Furthermore, Themis expects the send callback to send the whole message so it is kinda
    // pointless to return the amount of bytes send. The receive callback returns accurate number
    // of bytes, but I do not really like the Rust interface this implies. It could be made better.

    /// Sends a message to the remote peer.
    ///
    /// This method will fail if a secure connection has not been established yet.
    ///
    /// This method is a part of transport API and requires [`send_data`] method of
    /// `SecureSessionTransport`.
    ///
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    pub fn send<M: AsRef<[u8]>>(&mut self, message: M) -> Result<()> {
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        unsafe {
            let length =
                secure_session_send(self.session_ctx, message_ptr as *const c_void, message_len);
            if length <= 21 {
                return Err(Error::from_session_status(length as themis_status_t));
            }
        }

        Ok(())
    }

    /// Receives a message from the remote peer.
    ///
    /// Maximum length of the message is specified by the parameter. The message will be truncated
    /// to this length if the peer sends something larger.
    ///
    /// This method will fail if a secure connection has not been established yet.
    ///
    /// This method is a part of transport API and requires [`receive_data`] method of
    /// `SecureSessionTransport`.
    ///
    /// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
    pub fn receive(&mut self, max_len: usize) -> Result<Vec<u8>> {
        let mut message = Vec::with_capacity(max_len);

        unsafe {
            let length = secure_session_receive(
                self.session_ctx,
                message.as_mut_ptr() as *mut c_void,
                message.capacity(),
            );
            if length <= 21 {
                return Err(Error::from_session_status(length as themis_status_t));
            }
            debug_assert!(length as usize <= message.capacity());
            message.set_len(length as usize);
        }

        Ok(message)
    }

    /// Continues connection negotiation.
    ///
    /// This method performs one step of connection negotiation. This is the first method to call
    /// for the server, the client calls it after the initial [`connect`]. Both peers shall then
    /// repeatedly call this method until the connection is established (see [`connect`] for
    /// details).
    ///
    /// This method is a part of transport API and requires [`send_data`] and [`receive_data`]
    /// methods of `SecureSessionTransport`.
    ///
    /// [`connect`]: struct.SecureSession.html#method.connect
    /// [`send_data`]: trait.SecureSessionTransport.html#method.send_data
    /// [`receive_data`]: trait.SecureSessionTransport.html#method.receive_data
    pub fn negotiate_transport(&mut self) -> Result<()> {
        unsafe {
            let result = secure_session_receive(self.session_ctx, ptr::null_mut(), 0);
            let error = Error::from_session_status(result as themis_status_t);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
        }

        Ok(())
    }
}

impl<T> SecureSessionDelegate<T>
where
    T: SecureSessionTransport,
{
    pub fn new(transport: T) -> Box<Self> {
        let mut delegate = Box::new(Self {
            callbacks: secure_session_user_callbacks_t {
                send_data: Some(Self::send_data),
                receive_data: Some(Self::receive_data),
                state_changed: Some(Self::state_changed),
                get_public_key_for_id: Some(Self::get_public_key_for_id),
                user_data: ptr::null_mut(),
            },
            transport,
        });
        delegate.callbacks.user_data = delegate.transport_ptr();
        delegate
    }

    pub fn user_callbacks(&self) -> *const secure_session_user_callbacks_t {
        &self.callbacks
    }

    // These functions are unsafe. They should be used only for `user_data` conversion.

    fn transport_ptr(&mut self) -> *mut c_void {
        &mut self.transport as *mut T as *mut c_void
    }

    fn transport<'a>(ptr: *mut c_void) -> &'a mut T {
        unsafe { &mut *(ptr as *mut T) }
    }

    unsafe extern "C" fn send_data(
        data_ptr: *const u8,
        data_len: usize,
        user_data: *mut c_void,
    ) -> isize {
        let data = byte_slice_from_ptr(data_ptr, data_len);
        let transport = Self::transport(user_data);

        transport
            .send_data(data)
            .ok()
            .and_then(as_isize)
            .unwrap_or(-1)
    }

    unsafe extern "C" fn receive_data(
        data_ptr: *mut u8,
        data_len: usize,
        user_data: *mut c_void,
    ) -> isize {
        let data = byte_slice_from_ptr_mut(data_ptr, data_len);
        let transport = Self::transport(user_data);

        transport
            .receive_data(data)
            .ok()
            .and_then(as_isize)
            .unwrap_or(-1)
    }

    unsafe extern "C" fn state_changed(event: c_int, user_data: *mut c_void) {
        let transport = Self::transport(user_data);

        if let Some(state) = SecureSessionState::from_int(event) {
            transport.state_changed(state);
        }
    }

    unsafe extern "C" fn get_public_key_for_id(
        id_ptr: *const c_void,
        id_len: usize,
        key_ptr: *mut c_void,
        key_len: usize,
        user_data: *mut c_void,
    ) -> c_int {
        let id = byte_slice_from_ptr(id_ptr as *const u8, id_len);
        let key_out = byte_slice_from_ptr_mut(key_ptr as *mut u8, key_len);
        let transport = Self::transport(user_data);

        if let Some(key) = transport.get_public_key_for_id(id) {
            let key = key.as_ref();
            if key_out.len() >= key.len() {
                key_out[0..key.len()].copy_from_slice(key);
                return 0;
            }
        }
        -1
    }
}

#[doc(hidden)]
impl<D> Drop for SecureSession<D> {
    fn drop(&mut self) {
        unsafe {
            let status = secure_session_destroy(self.session_ctx);
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

fn byte_slice_from_ptr<'a>(ptr: *const u8, len: usize) -> &'a [u8] {
    unsafe { slice::from_raw_parts(escape_null_ptr(ptr as *mut u8), len) }
}

fn byte_slice_from_ptr_mut<'a>(ptr: *mut u8, len: usize) -> &'a mut [u8] {
    unsafe { slice::from_raw_parts_mut(escape_null_ptr(ptr), len) }
}

fn escape_null_ptr<T>(ptr: *mut T) -> *mut T {
    if ptr.is_null() {
        ptr::NonNull::dangling().as_ptr()
    } else {
        ptr
    }
}
