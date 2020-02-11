# WasmThemis

[![npm][npm-badge]][npm]
[![CircleCI][circle-ci-badge]][circle-ci]
[![License][license-badge]][license]

_WebAssembly_ wrapper for [Themis crypto library][themis].

Themis is a convenient cryptographic library for data protection. It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including WebAssembly, Node.js, Python, iOS/macOS, and Java/Android.

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

### Documentation

Read the following resources to learn more:

  - [How to use WasmThemis with JavaScript][language-guide].
  - [How to build WasmThemis from source code][build-instructions].
  - [General documentation for Themis library on Cossack Labs Documentation Server][docserver].

### Additional resources  
  - To get a better understanding of how to use Themis, try playing around with [Interactive Themis Server Simulator](https://docs.cossacklabs.com/simulator/interactive/).
  - If Node.js wrapper would be a better match for your project, see [JsThemis](https://github.com/cossacklabs/themis/tree/master/src/wrappers/themis/jsthemis).

<!--
TODO: refer code samples and tests here
-->

[language-guide]: https://docs.cossacklabs.com/pages/js-wasm-howto/
[build-instructions]: https://docs.cossacklabs.com/pages/documentation-themis/#webassembly-wrapper-installation
[docserver]: https://docs.cossacklabs.com/products/themis/

## Licensing

WasmThemis is distributed under [Apache 2.0 license](LICENSE).
