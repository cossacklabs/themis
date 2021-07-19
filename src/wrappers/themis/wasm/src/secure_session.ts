// Copyright (c) 2019 Cossack Labs Limited
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

/**
 * @file
 * Themis Secure Session.
 */

import { KeyPair, PublicKey, PrivateKey, SymmetricKey } from "./secure_keygen";
import context from "./context";
import { ThemisError, ThemisErrorCode } from "./themis_error";
import {
  coerceToBytes,
  heapFree,
  heapGetArray,
  heapPutArray,
  heapAlloc,
} from "./utils";

const cryptosystem_name = "SecureSession";

function isFunction(obj: any) {
  return !!(obj && obj.constructor && obj.call && obj.apply);
}

// Secure Session C API operatates with "secure_session_user_callbacks_t" context structure
// defined in <themis/secure_session.h>. It must be filled in with C function pointers and
// an arbitrary C context pointer. Associated C functions are called by Secure Session
// at appropriate moments. They receive the C context pointer as their last argument which
// allows to restore context.
//
// First, we need to be able to allocate and free "secure_session_user_callbacks_t", and to
// initialize it before passing them to Secure Session constructor.
//
// Here's how it looks in C:
//
//     struct secure_session_user_callbacks_type {
//          send_protocol_data_callback     send_data;
//          receive_protocol_data_callback  receive_data;
//          protocol_state_changed_callback state_changed;
//          get_public_key_for_id_callback  get_public_key_for_id;
//
//          void *user_data;
//     };
//
// On Emscripten target all pointers take up 4 bytes and are aligned at 4-byte boundary.
// Hence the following sizes and offsets:

const sizeof_secure_session_user_callbacks_t = 20;
const offsetof_user_data = 16;
const offsetof_get_public_key_for_id = 12;

// One does not simply pass JavaScript function into Emscripten memory. We need to use
// the "addFunction" API which registers a limited number of JavaScript functions and
// provides corresponding C function pointers to thunks. We need only one callback so
// it's relatively easy for us. We just need to ensure that the function is registered
// only once.
//
// As for the context, we'll save a pointer to "secure_session_user_callbacks_t" itself
// in its "user_data" field.

var getPublicKeyForIdThunkPtr = 0;

function initUserCallbacks(callbacksPtr: number) {
  if (getPublicKeyForIdThunkPtr == 0) {
    // LLVM needs to know the prototype of the corresponding C function.
    // See comment for "getPublicKeyForIdThunk" below.
    getPublicKeyForIdThunkPtr = context.libthemis!!.addFunction(
      getPublicKeyForIdThunk,
      "iiiiii"
    );
  }
  context.libthemis!!._memset(
    callbacksPtr,
    0,
    sizeof_secure_session_user_callbacks_t
  );
  // We're writing LLVM pointers into memory, hence the '*' type specifier
  context.libthemis!!.setValue(
    callbacksPtr + offsetof_get_public_key_for_id,
    getPublicKeyForIdThunkPtr,
    "*"
  );
  context.libthemis!!.setValue(
    callbacksPtr + offsetof_user_data,
    callbacksPtr,
    "*"
  );
}

// Now we need to be able to identify JavaScript Secure Session object associated with
// a particular "secure_session_user_callbacks_t" instance. Unfortunately, keeping another
// global registry in JavaScript is the only feasible choice because JavaScript object
// memory is managed completely opaquely.

var SecureSessionCallbackRegistry: {
  [callbackPtr: number]: SecureSession;
} = {};

const registerSecureSession = (
  callbacksPtr: number,
  session: SecureSession
) => {
  SecureSessionCallbackRegistry[callbacksPtr] = session;
};

const unregisterSecureSession = (callbacksPtr: number) => {
  delete SecureSessionCallbackRegistry[callbacksPtr];
};

const getSecureSession = (callbacksPtr: number): SecureSession =>
  SecureSessionCallbackRegistry[callbacksPtr];

// Finally, here's our JavaScript implementation of the C function callback.
// It has the following prototype:
//
//     int get_public_key_for_id_callback(
//         const void* id,   size_t id_length,
//         void* key_buffer, size_t key_buffer_length,
//         void* user_data
//     );
//
// It is expected to look up a public key corresponding to the provided session ID,
// write the key into provided buffer, and return zero on success. Non-zero return
// values are considered errors.
//
// The function accepts five integer argumentes and returns an integer. Hence its
// LLVM signature is "iiiiii".
//
// We can throw JavaScript exceptions from inside the function, thanks to Emscripten.

const GetPublicKeySuccess = 0;
const GetPublicKeyFailure = -1;

const getPublicKeyForIdThunk = (
  idPtr: number,
  idLen: number,
  keyPtr: number,
  keyLen: number,
  userData: number
) => {
  let id = context.libthemis!!.HEAPU8.slice(idPtr, idPtr + idLen);
  let session = getSecureSession(userData);

  let publicKey = session.keyCallback(id);
  if (publicKey == null) {
    return GetPublicKeyFailure;
  }

  if (!(publicKey instanceof PublicKey)) {
    throw new ThemisError(
      cryptosystem_name,
      ThemisErrorCode.INVALID_PARAMETER,
      "Secure Session callback must return PublicKey or null"
    );
  }
  if (publicKey.length > keyLen) {
    throw new ThemisError(
      cryptosystem_name,
      ThemisErrorCode.BUFFER_TOO_SMALL,
      "public key cannot fit into provided buffer"
    );
  }
  heapPutArray(publicKey, keyPtr);

  return GetPublicKeySuccess;
};

type KeyCallback = (id: Uint8Array) => PublicKey;

export class SecureSession {
  private sessionPtr: number | null;
  public readonly keyCallback: KeyCallback;
  private callbacksPtr: number | null;

  constructor(
    sessionID: Uint8Array,
    privateKey: PrivateKey,
    keyCallback: KeyCallback
  ) {
    sessionID = coerceToBytes(sessionID);
    if (sessionID.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "session ID must be not empty"
      );
    }
    if (!(privateKey instanceof PrivateKey)) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "expected PrivateKey as second argument"
      );
    }
    if (!isFunction(keyCallback)) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "expected callback as third argument"
      );
    }

    let session_id_ptr, private_key_ptr, callbacks_ptr;
    try {
      session_id_ptr = heapAlloc(sessionID.length);
      private_key_ptr = heapAlloc(privateKey.length);
      callbacks_ptr = heapAlloc(sizeof_secure_session_user_callbacks_t);
      if (!session_id_ptr || !private_key_ptr || !callbacks_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(sessionID, session_id_ptr);
      heapPutArray(privateKey, private_key_ptr);

      this.keyCallback = keyCallback;
      initUserCallbacks(callbacks_ptr);

      this.sessionPtr = context.libthemis!!._secure_session_create(
        session_id_ptr,
        sessionID.length,
        private_key_ptr,
        privateKey.length,
        callbacks_ptr
      );
      if (!this.sessionPtr) {
        throw new ThemisError(
          cryptosystem_name,
          ThemisErrorCode.NO_MEMORY,
          "failed to allocate Secure Session"
        );
      }

      registerSecureSession(callbacks_ptr, this);
      this.callbacksPtr = callbacks_ptr;
      callbacks_ptr = null;
    } finally {
      heapFree(private_key_ptr, privateKey.length);
      heapFree(session_id_ptr, sessionID.length);
      heapFree(callbacks_ptr, sizeof_secure_session_user_callbacks_t);
    }
  }

  destroy() {
    let status = context.libthemis!!._secure_session_destroy(this.sessionPtr!!);
    if (status != ThemisErrorCode.SUCCESS) {
      throw new ThemisError(
        cryptosystem_name,
        status,
        "failed to destroy Secure Session"
      );
    }
    this.sessionPtr = null;

    unregisterSecureSession(this.callbacksPtr!!);
    heapFree(this.callbacksPtr, sizeof_secure_session_user_callbacks_t);
    this.callbacksPtr = null;
  }

  established() {
    let value = context.libthemis!!._secure_session_is_established(
      this.sessionPtr!!
    );
    return !(value == 0);
  }

  connectionRequest() {
    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let request_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let request_ptr, request_length;
    try {
      status = context.libthemis!!._secure_session_generate_connect_request(
        this.sessionPtr!!,
        null,
        request_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      request_length = context.libthemis!!.getValue(request_length_ptr, "i32");
      request_ptr = heapAlloc(request_length);
      if (!request_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._secure_session_generate_connect_request(
        this.sessionPtr!!,
        request_ptr,
        request_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      request_length = context.libthemis!!.getValue(request_length_ptr, "i32");

      return heapGetArray(request_ptr, request_length);
    } finally {
      heapFree(request_ptr, request_length);
    }
  }

  negotiateReply(message: Uint8Array) {
    message = coerceToBytes(message);
    if (message.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "message must be not empty"
      );
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let reply_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let message_ptr, reply_ptr, reply_length;
    try {
      message_ptr = heapAlloc(message.length);
      if (!message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(message, message_ptr);

      status = context.libthemis!!._secure_session_unwrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        null,
        reply_length_ptr
      );
      if (status == ThemisErrorCode.SUCCESS) {
        return new Uint8Array();
      }
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      reply_length = context.libthemis!!.getValue(reply_length_ptr, "i32");
      reply_ptr = heapAlloc(reply_length);
      if (!reply_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._secure_session_unwrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        reply_ptr,
        reply_length_ptr
      );
      if (status != ThemisErrorCode.SSESSION_SEND_OUTPUT_TO_PEER) {
        throw new ThemisError(cryptosystem_name, status);
      }

      reply_length = context.libthemis!!.getValue(reply_length_ptr, "i32");

      return heapGetArray(reply_ptr, reply_length);
    } finally {
      heapFree(message_ptr, message.length);
      heapFree(reply_ptr, reply_length);
    }
  }

  wrap(message: Uint8Array) {
    message = coerceToBytes(message);
    if (message.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "message must be not empty"
      );
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let wrapped_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let message_ptr, wrapped_ptr, wrapped_length;
    try {
      message_ptr = heapAlloc(message.length);
      if (!message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(message, message_ptr);

      status = context.libthemis!!._secure_session_wrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        null,
        wrapped_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      wrapped_length = context.libthemis!!.getValue(wrapped_length_ptr, "i32");
      wrapped_ptr = heapAlloc(wrapped_length);
      if (!wrapped_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._secure_session_wrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        wrapped_ptr,
        wrapped_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      wrapped_length = context.libthemis!!.getValue(wrapped_length_ptr, "i32");

      return heapGetArray(wrapped_ptr, wrapped_length);
    } finally {
      heapFree(message_ptr, message.length);
      heapFree(wrapped_ptr, wrapped_length);
    }
  }

  unwrap(message: Uint8Array) {
    message = coerceToBytes(message);
    if (message.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "message must be not empty"
      );
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let unwrapped_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let message_ptr, unwrapped_ptr, unwrapped_length;
    try {
      message_ptr = heapAlloc(message.length);
      if (!message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(message, message_ptr);

      status = context.libthemis!!._secure_session_unwrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        null,
        unwrapped_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      unwrapped_length = context.libthemis!!.getValue(
        unwrapped_length_ptr,
        "i32"
      );
      unwrapped_ptr = heapAlloc(unwrapped_length);
      if (!unwrapped_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._secure_session_unwrap(
        this.sessionPtr!!,
        message_ptr,
        message.length,
        unwrapped_ptr,
        unwrapped_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      unwrapped_length = context.libthemis!!.getValue(
        unwrapped_length_ptr,
        "i32"
      );

      return heapGetArray(unwrapped_ptr, unwrapped_length);
    } finally {
      heapFree(message_ptr, message.length);
      heapFree(unwrapped_ptr, unwrapped_length);
    }
  }
}
