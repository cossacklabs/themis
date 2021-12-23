# JsThemis

[![npm][npm-badge]][npm]
[![JsThemis][github-ci-badge]][github-ci]
[![License][license-badge]][license]

_Node.js_ wrapper for [Themis crypto library](https://github.com/cossacklabs/themis).

Themis is a convenient cryptographic library for data protection. 
It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including Node.js, WebAssembly, Python, iOS/macOS, and Java/Android.

By [Cossack Labs](https://www.cossacklabs.com/themis/).

[npm]: https://www.npmjs.com/package/jsthemis
[npm-badge]: https://img.shields.io/npm/v/jsthemis.svg
[github-ci]: https://github.com/cossacklabs/themis/actions?query=workflow%3AJsThemis
[github-ci-badge]: https://github.com/cossacklabs/themis/workflows/JsThemis/badge.svg
[license]: LICENSE
[license-badge]: https://img.shields.io/npm/l/jsthemis.svg

## Getting started

### Installation

JsThemis requires native Themis library to be installed.
Please refer to the [installation instructions](https://docs.cossacklabs.com/themis/languages/nodejs/installation/).

After that install the latest version of JsThemis:

```
npm install jsthemis
```

Import it into your project:

```javascript
const themis = require('jsthemis')
```

That's it!

### Documentation

Read the following resources to learn more:

  - [How to use JsThemis with JavaScript](https://docs.cossacklabs.com/themis/languages/nodejs/).
  - [General documentation for Themis library on Cossack Labs Documentation Server](https://docs.cossacklabs.com/themis/).

### Additional resources  
  - If WebAssembly would be a better match for your project, see [WasmThemis wrapper](https://www.npmjs.com/package/wasm-themis).

## Licensing

JsThemis is distributed under [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).
