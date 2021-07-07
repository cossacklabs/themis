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

export { SecureCellSeal } from "./secure_cell_seal";
export { SecureCellTokenProtect } from "./secure_cell_token_protect";
export { SecureCellContextImprint } from "./secure_cell_context_imprint";
export { ThemisError, ThemisErrorCode } from "./themis_error";
export {
  SecureMessageSign,
  SecureMessage,
  SecureMessageVerify,
} from "./secure_message";
export { SecureSession } from "./secure_session";
export { KeyPair, PrivateKey, PublicKey, SymmetricKey } from "./secure_keygen";
export { SecureComparator } from "./secure_comparator";

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

/**
 * Initialize WasmThemis.
 *
 * This function **must** be called and awaited before using any WasmThemis interfaces.
 * It will download and compile WebAssembly code of WasmThemis.
 *
 * If you are hosting WebAssembly code on a CDN or at a non-standard location,
 * you should pass the URL as an argument.
 * Otherwise, `libthemis.wasm` will be downloaded, relative to the script.
 *
 * @param wasmPath URL of `libthemis.wasm` to download.
 *
 * @since WasmThemis 0.14.0
 */
export const initialize = async (wasmPath?: string) => {
  context.libthemis = await libthemisFn({
    onRuntimeInitialized: () => onRuntimeInitialized(),
    locateFile: wasmPath ? function () {
      return wasmPath;
    } : undefined,
  });

  return context.libthemis;
};

// However, it was not always the case. Previously, WasmThemis has exported
// just "initialzed" promise which is resolved once WASM code is downloaded
// and compiled. User code is expected to await for that promise to resolve,
// then proceed using WasmThemis functions.
//
// Back in the day, WasmThemis was not modularized, so the download & compile
// was initialized immediately once JS code of WasmThemis got loaded and the
// module was evalated.
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

// TypeScript does not allow to extend Promise nicely [1], but since JavaScript
// is actually duck-typed, we can just mimick the API and get away with it.
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
 * Await WasmThemis initialisation.
 *
 * This promise is resolved once WebAssembly code has been downloaded and compiled
 * and WasmThemis is ready to use.
 *
 * You **must** await either this promise, or `initialize()` to use WasmThemis.
 *
 * WebAssembly code is expected to be located at `libthemis.wasm` relative to the script.
 * If you need to use a custom location, call `initialize()`.
 *
 * @see initialize
 */
export const initialized = new InitializedPromise((resolve) => {
  onRuntimeInitialized = resolve;
});
