# Themis React Native 

[![npm][npm-badge]][npm]
[![JsThemis][github-ci-badge]][github-ci]
[![License][license-badge]][license]

_React Native_ wrapper for [Themis crypto library](https://github.com/cossacklabs/themis).

Themis is a convenient cryptographic library for data protection. 
It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including Node.js, WebAssembly, Python, iOS/macOS, and Java/Android.

By [Cossack Labs](https://www.cossacklabs.com/themis/).

[npm]: https://www.npmjs.com/package/react-native-themis
[github-ci]: https://github.com/cossacklabs/themis/actions?query=workflow%3AJsThemis
[github-ci-badge]: https://github.com/cossacklabs/themis/workflows/JsThemis/badge.svg
[license]: LICENSE
[license-badge]: https://img.shields.io/npm/l/jsthemis.svg

## Getting started

### Installation

```
npm install --save react-native-themis
cd ios
pod install 
```

Import it into your project:

```javascript
import {
  keyPair64,
  symmetricKey64,
  secureCellSealWithSymmetricKeyEncrypt64,
  secureCellSealWithSymmetricKeyDecrypt64,
  secureCellSealWithPassphraseEncrypt64,
  secureCellSealWithPassphraseDecrypt64,
  secureCellTokenProtectEncrypt64,
  secureCellTokenProtectDecrypt64,
  secureCellContextImprintEncrypt64,
  secureCellContextImprintDecrypt64,
  secureMessageSign64,
  secureMessageVerify64,
  secureMessageEncrypt64,
  secureMessageDecrypt64,
  string64,
  comparatorInit64,
  comparatorBegin,
  comparatorProceed64,
  KEYTYPE_EC,
  KEYTYPE_RSA,
  COMPARATOR_NOT_READY,
  COMPARATOR_NOT_MATCH,
  COMPARATOR_MATCH,
  COMPARATOR_ERROR
} from 'react-native-themis'
```
That's it!

### Documentation

Read the following resources to learn more:

  - [How to use Themis with React Native](https://docs.cossacklabs.com/themis/languages/react-native/).
  - [General documentation for Themis library on Cossack Labs Documentation Server](https://docs.cossacklabs.com/themis/).


## Licensing

Themis for React Native is distributed under [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).
