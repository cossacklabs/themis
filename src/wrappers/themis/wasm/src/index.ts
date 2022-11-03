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

import context from "./context";
import libthemisFn from "./libthemis.js";

import { SecureCellSeal } from "./secure_cell_seal";
import { SecureCellTokenProtect } from "./secure_cell_token_protect";
import { SecureCellContextImprint } from "./secure_cell_context_imprint";
import { ThemisError, ThemisErrorCode } from "./themis_error";
import {
  SecureMessageSign,
  SecureMessage,
  SecureMessageVerify,
} from "./secure_message";
import { SecureSession } from "./secure_session";
import { KeyPair, PrivateKey, PublicKey, SymmetricKey } from "./secure_keygen";
import { SecureComparator } from "./secure_comparator";

// WebAssembly code is not directly included with compiled JavaScript code.
// Emscripten generates a stub that will download and compile WebAssembly
// after this module is loaded. This initialization is necessary before
// any WasmThemis function can be called.
//
// Due to historical reasons, WasmThemis has multiple ways to initialize.

let onRuntimeInitialized: () => void

// The current facility is "async initialize()" function which user code
// is expected to call, then await the result to resolve before using
// other WasmThemis functions.

let libthemisInitialized = false;

/**
 * Initialize WasmThemis.
 *
 * This function **must** be called and awaited before using any WasmThemis interfaces.
 * It will download and compile WebAssembly code of WasmThemis.
 *
 * If you are hosting `libthemis.wasm` on a CDN or at a non-standard location,
 * pass URL to `libthemis.wasm` as an argument.
 * If URL is omitted, `libthemis.wasm` is expected to be located in the same directory
 * as the executing script.
 *
 * @param wasmPath URL of `libthemis.wasm` to download.
 *
 * @throws {ThemisError} is thrown if this function is called more than once,
 * or if WasmThemis has been already initialized via `initialized`.
 *
 * @since WasmThemis 0.14.0
 */
export const initialize = async (wasmPath?: string) => {
  if (libthemisInitialized) {
    throw new ThemisError(
      'initialize',
      ThemisErrorCode.FAIL,
      'WasmThemis can only be initalized once',
    );
  }
  context.libthemis = await libthemisFn({
    onRuntimeInitialized: () => onRuntimeInitialized(),
    locateFile: wasmPath ? function () {
      return wasmPath;
    } : undefined,
  });
  libthemisInitialized = true;

  return context.libthemis;
};

// However, it was not always the case. Previously, WasmThemis has exported
// just "initialized" promise which is resolved once WASM code is downloaded
// and compiled. User code is expected to await for that promise to resolve,
// then proceed using WasmThemis functions.
//
// Back in the day, WasmThemis was not modularized, so the download & compile
// was initiated immediately once JS code of WasmThemis got loaded and the
// module was evaluated.
//
// Obviously, there is no way to pass any parameters to this promise either.
//
// In order to keep "initialized" working, we do some trickery, exporting
// a promise that will make sure that initialize() is called before this
// promise is resolved. This kickstarts WebAssmebly loading and ensures
// that WasmThemis is initialized once "initialized" promise is resolved.
//
// This approach does not cover the case where the user code does not use
// "initialized" promise at all. That is, if the application just hopes
// that WasmThemis is loaded by the time it's needed. This should be rare.
// Users have been warned.

// TypeScript does not allow to extend Promise nicely in ES5 [1], but since
// JavaScript is actually duck-typed, we can just mimic the API.
// [1]: https://github.com/microsoft/TypeScript/issues/15202
class InitializedPromise {
  private initialized: boolean = false;
  private readonly promise: Promise<void>;

  constructor(executor: (resolve: (value: void | PromiseLike<void>) => void,
                         reject: (reason?: any) => void) => void)
  {
    this.promise = new Promise(executor);
  }

  private initialize() {
    // Make sure that initialize() -- the exported one -- is only called once.
    // Promises can have their then() and catch() methods called multiple times
    // to register multiple callbacks. Register callbacks, but call initialize()
    // only once. Also, it's okay to ignore its result. For the "initialized"
    // code path we register "onRuntimeInitialized" callback that will resolve
    // *this* promise when it is time.
    if (!this.initialized) {
      initialize();
      this.initialized = true;
    }
  }

  then<T = any, E = never>(
    fulfilled?: ((value: any) => T | PromiseLike<T>) | null | undefined,
    rejected?: ((reason: any) => E | PromiseLike<E>) | null | undefined,
  ) : Promise<T | E>
  {
    this.initialize();
    return this.promise.then(fulfilled, rejected);
  }

  catch<T = never>(
    rejected?: ((reason: any) => T | PromiseLike<T>) | undefined | null,
  ): Promise<void | T>
  {
    this.initialize();
    return this.promise.catch(rejected);
  }
}

/**
 * Await WasmThemis initialization.
 *
 * This promise is resolved once WebAssembly code has been downloaded and compiled
 * and WasmThemis is ready to use.
 *
 * You **must** await either this promise or `initialize()` to use WasmThemis.
 *
 * WebAssembly code is expected to be located in `libthemis.wasm` file
 * in the same directory as the executing script.
 * If you need to use a custom location, call `initialize()`.
 *
 * Note that you cannot use `initialize()` after WasmThemis has been initialized
 * using this promise.
 *
 * @see initialize
 */
export const initialized = new InitializedPromise((resolve) => {
  onRuntimeInitialized = resolve;
});

// Historically, WasmThemis allowed to use default import in ES6 code:
//
//     import themis from 'wasm-themis';
//
// Make sure it still works.

export default {
  initialize,
  initialized,
  SecureCellSeal,
  SecureCellTokenProtect,
  SecureCellContextImprint,
  SecureMessage,
  SecureMessageSign,
  SecureMessageVerify,
  SecureSession,
  SecureComparator,
  ThemisError,
  ThemisErrorCode,
  SymmetricKey,
  KeyPair,
  PrivateKey,
  PublicKey,
};

// And of course export all public things individually.

export {
  SecureCellSeal,
  SecureCellTokenProtect,
  SecureCellContextImprint,
  SecureMessage,
  SecureMessageSign,
  SecureMessageVerify,
  SecureSession,
  SecureComparator,
  ThemisError,
  ThemisErrorCode,
  SymmetricKey,
  KeyPair,
  PrivateKey,
  PublicKey,
};
