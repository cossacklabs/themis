# WasmThemis

[![npm][npm-badge]][npm]
[![WasmThemis][github-ci-badge]][github-ci]
[![License][license-badge]][license]

_WebAssembly_ wrapper for [Themis crypto library][themis].

Themis is a convenient cryptographic library for data protection. It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including WebAssembly, Node.js, Python, iOS/macOS, and Java/Android.

[themis]: https://github.com/cossacklabs/themis
[npm]: https://www.npmjs.com/package/wasm-themis
[npm-badge]: https://img.shields.io/npm/v/wasm-themis.svg
[github-ci]: https://github.com/cossacklabs/themis/actions?query=workflow%3AWasmThemis
[github-ci-badge]: https://github.com/cossacklabs/themis/workflows/WasmThemis/badge.svg
[license]: LICENSE
[license-badge]: https://img.shields.io/npm/l/wasm-themis.svg

## Getting started

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
  - [How to deploy apps using WasmThemis][deploy].
  - [General documentation for Themis library on Cossack Labs Documentation Server][docserver].

### Additional resources  
  - If Node.js wrapper would be a better match for your project, see [JsThemis](https://www.npmjs.com/package/jsthemis).

<!--
TODO: refer code samples and tests here
-->

[language-guide]: https://docs.cossacklabs.com/themis/languages/wasm/
[deploy]: https://docs.cossacklabs.com/themis/languages/wasm/installation/
[docserver]: https://docs.cossacklabs.com/themis/

## Licensing

WasmThemis is distributed under [Apache 2.0 license](LICENSE).
