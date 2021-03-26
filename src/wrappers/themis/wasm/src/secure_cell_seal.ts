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
 * Themis Secure Cell (Seal mode).
 */

import themisContext from "./context";
import { ThemisError, ThemisErrorCode } from "./themis_error";
import {
  coerceToBytes,
  heapFree,
  heapGetArray,
  heapPutArray,
  heapAlloc,
  passphraseBytes,
} from "./utils";

const cryptosystem_name = "SecureCellSeal";

export class SecureCellSeal {
  protected masterKey: Uint8Array;

  constructor(masterKey: Uint8Array) {
    masterKey = coerceToBytes(masterKey);
    if (masterKey.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "master key must be not empty"
      );
    }
    this.masterKey = masterKey;
  }

  /**
   * Makes a new Secure Cell in Seal mode with given master key.
   *
   * @param masterKey     non-empty array of master key bytes (Buffer or Uint8Array)
   *
   * @returns a new instance of SecureCellSeal.
   *
   * @throws TypeError if the master key is not a byte buffer.
   * @throws ThemisError if the master key is empty.
   */
  static withKey(masterKey: Uint8Array) {
    return new SecureCellSeal(masterKey);
  }

  /**
   * Makes a new Secure Cell in Seal mode with given passphrase.
   *
   * @param passphrase    non-empty string with passphrase,
   *                      or a non-empty raw byte array (Buffer or Uint8Array)
   *
   * @returns a new instance of SecureCellSeal.
   *
   * @throws TypeError if the passphrase is not a string or a byte buffer.
   * @throws ThemisError if the passphrase is empty.
   */
  static withPassphrase(passphrase: string) {
    return new SecureCellSealWithPassphrase(passphrase);
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

    let context;
    if (arguments.length > 1 && arguments[1] !== null) {
      context = coerceToBytes(arguments[1]);
    } else {
      context = new Uint8Array();
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let result_length_ptr = themisContext.libthemis!!.allocate(
      new ArrayBuffer(4),
      themisContext.libthemis!!.ALLOC_STACK
    );
    let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length;
    try {
      master_key_ptr = heapAlloc(this.masterKey.length);
      message_ptr = heapAlloc(message.length);
      context_ptr = heapAlloc(context.length);
      if (!master_key_ptr || !message_ptr || !context_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.masterKey, master_key_ptr);
      heapPutArray(message, message_ptr);
      heapPutArray(context, context_ptr);

      status = themisContext.libthemis!!._themis_secure_cell_encrypt_seal(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = themisContext.libthemis!!._themis_secure_cell_encrypt_seal(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(master_key_ptr, this.masterKey.length);
      heapFree(message_ptr, message.length);
      heapFree(context_ptr, context.length);
      heapFree(result_ptr, result_length);
    }
  }

  decrypt(message: Uint8Array, context: Uint8Array = new Uint8Array()) {
    message = coerceToBytes(message);
    if (message.length == 0) {
      throw new ThemisError(
        cryptosystem_name,
        ThemisErrorCode.INVALID_PARAMETER,
        "message must be not empty"
      );
    }

    context = coerceToBytes(context);

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let result_length_ptr = themisContext.libthemis!!.allocate(
      new ArrayBuffer(4),
      themisContext.libthemis!!.ALLOC_STACK
    );
    let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length;
    try {
      master_key_ptr = heapAlloc(this.masterKey.length);
      message_ptr = heapAlloc(message.length);
      context_ptr = heapAlloc(context.length);
      if (!master_key_ptr || !message_ptr || !context_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.masterKey, master_key_ptr);
      heapPutArray(message, message_ptr);
      heapPutArray(context, context_ptr);

      status = themisContext.libthemis!!._themis_secure_cell_decrypt_seal(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = themisContext.libthemis!!._themis_secure_cell_decrypt_seal(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(master_key_ptr, this.masterKey.length);
      heapFree(message_ptr, message.length);
      heapFree(context_ptr, context.length);
      heapFree(result_ptr, result_length);
    }
  }
}

class SecureCellSealWithPassphrase extends SecureCellSeal {
  constructor(passphrase: string) {
    super(passphraseBytes(passphrase));
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

    let context;
    if (arguments.length > 1 && arguments[1] !== null) {
      context = coerceToBytes(arguments[1]);
    } else {
      context = new Uint8Array();
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let result_length_ptr = themisContext.libthemis!!.allocate(
      new ArrayBuffer(4),
      themisContext.libthemis!!.ALLOC_STACK
    );
    let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length;
    try {
      master_key_ptr = heapAlloc(this.masterKey.length);
      message_ptr = heapAlloc(message.length);
      context_ptr = heapAlloc(context.length);
      if (!master_key_ptr || !message_ptr || !context_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.masterKey, master_key_ptr);
      heapPutArray(message, message_ptr);
      heapPutArray(context, context_ptr);

      status = themisContext.libthemis!!._themis_secure_cell_encrypt_seal_with_passphrase(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = themisContext.libthemis!!._themis_secure_cell_encrypt_seal_with_passphrase(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(master_key_ptr, this.masterKey.length);
      heapFree(message_ptr, message.length);
      heapFree(context_ptr, context.length);
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

    let context;
    if (arguments.length > 1 && arguments[1] !== null) {
      context = coerceToBytes(arguments[1]);
    } else {
      context = new Uint8Array();
    }

    let status;
    /// C API uses "size_t" for lengths, it's defined as "i32" in Emscripten
    let result_length_ptr = themisContext.libthemis!!.allocate(
      new ArrayBuffer(4),
      themisContext.libthemis!!.ALLOC_STACK
    );
    let master_key_ptr, message_ptr, context_ptr, result_ptr, result_length;
    try {
      master_key_ptr = heapAlloc(this.masterKey.length);
      message_ptr = heapAlloc(message.length);
      context_ptr = heapAlloc(context.length);
      if (!master_key_ptr || !message_ptr || !context_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      heapPutArray(this.masterKey, master_key_ptr);
      heapPutArray(message, message_ptr);
      heapPutArray(context, context_ptr);

      status = themisContext.libthemis!!._themis_secure_cell_decrypt_seal_with_passphrase(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        null,
        result_length_ptr
      );
      if (status != ThemisErrorCode.BUFFER_TOO_SMALL) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );
      result_ptr = heapAlloc(result_length);
      if (!result_ptr) {
        throw new ThemisError(cryptosystem_name, ThemisErrorCode.NO_MEMORY);
      }

      status = themisContext.libthemis!!._themis_secure_cell_decrypt_seal_with_passphrase(
        master_key_ptr,
        this.masterKey.length,
        context_ptr,
        context.length,
        message_ptr,
        message.length,
        result_ptr,
        result_length_ptr
      );
      if (status != ThemisErrorCode.SUCCESS) {
        throw new ThemisError(cryptosystem_name, status);
      }

      result_length = themisContext.libthemis!!.getValue(
        result_length_ptr,
        "i32"
      );

      return heapGetArray(result_ptr, result_length);
    } finally {
      heapFree(master_key_ptr, this.masterKey.length);
      heapFree(message_ptr, message.length);
      heapFree(context_ptr, context.length);
      heapFree(result_ptr, result_length);
    }
  }
}
