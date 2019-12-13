# WasmThemis

[![npm][npm-badge]][npm]
[![CircleCI][circle-ci-badge]][circle-ci]
[![License][license-badge]][license]

_WebAssembly_ wrapper for [**Themis** crypto library][themis].

Themis is an open-source high-level cryptographic services library that provides secure data exchange, authentication, and storage protection.
Themis provides ready-made building components, which simplifies the usage of core cryptographic security operations.

[themis]: https://github.com/cossacklabs/themis
[npm]: https://www.npmjs.com/package/wasm-themis
[npm-badge]: https://img.shields.io/npm/v/wasm-themis.svg
[circle-ci]: https://circleci.com/gh/cossacklabs/themis/tree/master
[circle-ci-badge]: https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield
[license]: LICENSE
[license-badge]: https://img.shields.io/npm/l/wasm-themis.svg

## Quickstart

### Installation

Start by installing the latest version of WasmThemis:

```
npm install wasm-themis
```

Import it into your project:

```javascript
const themis = require('wasm-themis')
```

Wait for WebAssembly code to load:

```javascript
themis.initialized.then(function() {
    //
    // Now you can use "themis" functions
    //
})
```
Success!

### Building

    WARNING: Building Themis wrappers from source is not recommended for casual or first-time use! Please install WebAssembly wrapper for Themis from repository unless you know why exactly you need to build your Themis wrapper from the source code.

    WARNING! WasmThemis wrapper cannot be built from source for i386 architecture, Debian 8 "Jessie" (and below), CentOS 7 (and below). But you can still install it on these operating systems using the following instruction.

Make sure that you have a working Emscripten installation and that it is activated in your environment:

```
$ emcc -v
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 1.38.30
clang version 6.0.1  (emscripten 1.38.30 : 1.38.30)
Target: x86_64-apple-darwin18.2.0
```

If you haven't used Emscripten before, take a look at this tutorial.

Next, make sure that BoringSSL submodule is checked out:

```
git submodule sync && git submodule update --init
```

Now you can build WasmThemis. The following command will build and package WasmThemis wrapper, core Themis library, and BoringSSL cryptographic backend:

```
emmake make wasmthemis
```

To run the test suite for WasmThemis wrapper (and a more extensive one for the core library), type:

```
emmake make test_wasm
emmake make test
```

To install WasmThemis, type:

```
make wasmthemis_install
```

### Documentation

Read the following resources to learn more:

  - [How to use WasmThemis with JavaScript][language-guide].
  - [General documentation for Themis library on Cossack Labs Documentation Server][docserver].

<!--

TODO: refer to simulators, code samples, and tests here

-->

[language-guide]: https://docs.cossacklabs.com/pages/js-wasm-howto/
[docserver]: https://docs.cossacklabs.com/products/themis/

## Licensing

WasmThemis is distributed under [Apache 2.0 license](LICENSE).
