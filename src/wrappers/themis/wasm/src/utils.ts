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
 * Miscellaneous utilities.
 */

import context from "./context";

/**
 * Convert an object into a byte buffer.
 *
 * @throws TypeError if input type is not supported
 */
export const coerceToBytes = (buffer: Uint8Array | ArrayBuffer): Uint8Array => {
  if (buffer instanceof Uint8Array) {
    return buffer;
  }
  if (buffer instanceof ArrayBuffer) {
    return new Uint8Array(buffer);
  }
  throw new TypeError('type mismatch, expect "Uint8Array" or "ArrayBuffer"');
};


const getTextEncoder = () => {
  if (typeof window !== "undefined" && window.TextEncoder) {
    return new window.TextEncoder();
  }

  if (typeof TextEncoder !== "undefined") {
    return new TextEncoder();
  }

  // we are using a browser which does not support TextEncoder or in a Node process which has TextEncoder
  // available through the util package.
  const NodeTextEncoder = require("util").TextEncoder;
  return new NodeTextEncoder();
};

const textEncoder = getTextEncoder();

const stringToUTF8 = (str: string) => textEncoder.encode(str);

/**
 * Convert passphrase into a byte buffer.
 *
 * @throws TypeError if input type is not supported
 */
export const passphraseBytes = (
  passphrase: string | Uint8Array | ArrayBuffer
) => {
  if (typeof passphrase === "string") {
    return stringToUTF8(passphrase);
  }
  if (passphrase instanceof Uint8Array) {
    return passphrase;
  }
  if (passphrase instanceof ArrayBuffer) {
    return new Uint8Array(passphrase);
  }
  throw new TypeError(
    'type mismatch, expect "string" or "Uint8Array", "ArrayBuffer"'
  );
};

/**
 * Allocate a buffer of specified length on Emscripten heap.
 */
export const heapAlloc = (length: number) => {
  // calloc() in not provided by Emscripten
  let buffer = context.libthemis!!._malloc(length);
  if (!!buffer) {
    context.libthemis!!._memset(buffer, 0, length);
  }
  return buffer;
};

/**
 * Move an array into Emscripten heap from JavaScript heap.
 */
export const heapPutArray = (array: number[] | Uint8Array, buffer: number) => {
  if (array instanceof Uint8Array) {
    context.libthemis!!.writeArrayToMemory(Array.from(array), buffer);
    return;
  }

  context.libthemis!!.writeArrayToMemory(array, buffer);
};

/**
 * Move an array from Emscripten heap into JavaScript heap.
 */
export const heapGetArray = (buffer: number, length: number): Uint8Array =>
  context.libthemis!!.HEAPU8.slice(buffer, buffer + length);

/**
 * Free a buffer on Emscripten heap.
 */
export const heapFree = (
  buffer: number | undefined | null,
  length: number | undefined
) => {
  // Prevent sensitive data leakage througn heap:
  if (length && buffer) {
    context.libthemis!!._memset(buffer, 0, length);
  }
  if (buffer) {
    context.libthemis!!._free(buffer);
  }
};
