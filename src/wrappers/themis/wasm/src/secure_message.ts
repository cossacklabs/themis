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
 * Themis Secure Message.
 */

import { KeyPair, PublicKey, PrivateKey } from "./secure_keygen";
import context from "./context";
import { ThemisError, ThemisErrorCode } from "./themis_error";
import {
  coerceToBytes,
  heapFree,
  heapGetArray,
  heapPutArray,
  heapAlloc,
} from "./utils";

const cryptosystem_name = "SecureMessage";

export class SecureMessage {
  private privateKey: PrivateKey;
  private publicKey: PublicKey;

  constructor(keyPair: KeyPair) {
    if (arguments.length == 1) {
      if (!(keyPair instanceof KeyPair)) {
        throw new ThemisError(
          cryptosystem_name,
          ThemisErrorCode.INVALID_PARAMETER,
          "invalid argument: must be KeyPair"
        );
      }
      this.privateKey = keyPair.privateKey;
      this.publicKey = keyPair.publicKey;
      return;
    }

    if (arguments.length == 2) {
      let arg0isPrivateKey = arguments[0] instanceof PrivateKey;
      let arg0isPublicKey = arguments[0] instanceof PublicKey;
      let arg1isPrivateKey = arguments[1] instanceof PrivateKey;
      let arg1isPublicKey = arguments[1] instanceof PublicKey;

      if (arg0isPublicKey && arg1isPrivateKey) {
        this.publicKey = arguments[0];
        this.privateKey = arguments[1];
        return;
      }
      if (arg0isPrivateKey && arg1isPublicKey) {
        this.privateKey = arguments[0];
        this.publicKey = arguments[1];
        return;
      }

      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "invalid arguments: expected PrivateKey and PublicKey"
      );
    }

    throw new ThemisError(
      cryptosystem_name,
      ThemisErrorCode.INVALID_PARAMETER,
      "invalid argument count: expected either one KeyPair, or PrivateKey and PublicKey"
    );
  }

  encrypt(message: Uint8Array) {
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
    let result_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let private_key_ptr, public_key_ptr, message_ptr, result_ptr, result_length;
    try {
      private_key_ptr = heapAlloc(this.privateKey.length);
      public_key_ptr = heapAlloc(this.publicKey.length);
      message_ptr = heapAlloc(message.length);
      if (!private_key_ptr || !public_key_ptr || !message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.privateKey, private_key_ptr);
      heapPutArray(this.publicKey, public_key_ptr);
      heapPutArray(message, message_ptr);

      status = context.libthemis!!._themis_secure_message_encrypt(
        private_key_ptr,
        this.privateKey.length,
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._themis_secure_message_encrypt(
        private_key_ptr,
        this.privateKey.length,
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(private_key_ptr, this.privateKey.length);
      heapFree(public_key_ptr, this.publicKey.length);
      heapFree(message_ptr, message.length);
      heapFree(result_ptr, result_length);
    }
  }

  decrypt(message: Uint8Array) {
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
    let result_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let private_key_ptr, public_key_ptr, message_ptr, result_ptr, result_length;
    try {
      private_key_ptr = heapAlloc(this.privateKey.length);
      public_key_ptr = heapAlloc(this.publicKey.length);
      message_ptr = heapAlloc(message.length);
      if (!private_key_ptr || !public_key_ptr || !message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.privateKey, private_key_ptr);
      heapPutArray(this.publicKey, public_key_ptr);
      heapPutArray(message, message_ptr);

      status = context.libthemis!!._themis_secure_message_decrypt(
        private_key_ptr,
        this.privateKey.length,
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._themis_secure_message_decrypt(
        private_key_ptr,
        this.privateKey.length,
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(private_key_ptr, this.privateKey.length);
      heapFree(public_key_ptr, this.publicKey.length);
      heapFree(message_ptr, message.length);
      heapFree(result_ptr, result_length);
    }
  }
}

export class SecureMessageSign {
  private privateKey: PrivateKey;

  constructor(privateKey: PrivateKey) {
    if (!(privateKey instanceof PrivateKey)) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "invalid argument: expected PrivateKey"
      );
    }
    this.privateKey = privateKey;
  }

  sign(message: Uint8Array) {
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
    let result_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let private_key_ptr, message_ptr, result_ptr, result_length;
    try {
      private_key_ptr = heapAlloc(this.privateKey.length);
      message_ptr = heapAlloc(message.length);
      if (!private_key_ptr || !message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.privateKey, private_key_ptr);
      heapPutArray(message, message_ptr);

      status = context.libthemis!!._themis_secure_message_sign(
        private_key_ptr,
        this.privateKey.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._themis_secure_message_sign(
        private_key_ptr,
        this.privateKey.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(private_key_ptr, this.privateKey.length);
      heapFree(message_ptr, message.length);
      heapFree(result_ptr, result_length);
    }
  }
}

export class SecureMessageVerify {
  private publicKey: PublicKey;
  constructor(publicKey: PublicKey) {
    if (!(publicKey instanceof PublicKey)) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "invalid argument: expected PublicKey"
      );
    }
    this.publicKey = publicKey;
  }

  verify(message: Uint8Array) {
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
    let result_length_ptr = context.libthemis!!.allocate(
      new ArrayBuffer(4),
      context.libthemis!!.ALLOC_STACK
    );
    let public_key_ptr, message_ptr, result_ptr, result_length;
    try {
      public_key_ptr = heapAlloc(this.publicKey.length);
      message_ptr = heapAlloc(message.length);
      if (!public_key_ptr || !message_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.publicKey, public_key_ptr);
      heapPutArray(message, message_ptr);

      status = context.libthemis!!._themis_secure_message_verify(
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = context.libthemis!!._themis_secure_message_verify(
        public_key_ptr,
        this.publicKey.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = context.libthemis!!.getValue(result_length_ptr, "i32");

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(public_key_ptr, this.publicKey.length);
      heapFree(message_ptr, message.length);
      heapFree(result_ptr, result_length);
    }
  }
}
