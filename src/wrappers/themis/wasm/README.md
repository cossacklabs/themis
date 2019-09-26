# WasmThemis

[![npm][npm-badge]][npm]
[![CircleCI][circle-ci-badge]][circle-ci]
[![License][license-badge]][license]

_WebAssembly_ wrapper for [**Themis** crypto library][themis].

Themis is an open-source high-level cryptographic services library
that provides secure data exchange, authentication, and storage protection.
Themis provides ready-made building components,
which simplify usage of core cryptographic security operations.

[themis]: https://github.com/cossacklabs/themis
[npm]: https://www.npmjs.com/package/wasm-themis
[npm-badge]: https://img.shields.io/npm/v/wasm-themis.svg
[circle-ci]: https://circleci.com/gh/cossacklabs/themis/tree/master
[circle-ci-badge]: https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield
[license]: LICENSE
[license-badge]: https://img.shields.io/npm/l/wasm-themis.svg

## Quickstart

### Installation

First, install the latest version of WasmThemis:

```
npm install wasm-themis
```

Then import it into your project:

```javascript
const themis = require('wasm-themis')
```

Finally, wait for WebAssembly code to be loaded:

```javascript
themis.initialized.then(function() {
    //
    // Now you can use "themis" functions
    //
})
```

### Documentation

Read the following resources to learn more:

  - [How to use WasmThemis with JavaScript][language-guide]
  - [General documentation for Themis library][docserver]

<!--

TODO: refer to simulators, code samples, and tests here

-->

[language-guide]: https://docs.cossacklabs.com/pages/js-wasm-howto/
[docserver]: https://docs.cossacklabs.com/products/themis/

## Licensing

WasmThemis is distributed under [Apache 2.0 license](LICENSE).
