<h3 align="center">
  <a href="https://www.cossacklabs.com/themis/"><img src="https://github.com/cossacklabs/themis/wiki/images/logo.png" alt="Themis provides strong, usable cryptography for busy people" width="420"></a>
  <br>
  <br>
  Themis provides strong, usable cryptography for busy people
  <br>
  <br>
</h3>

---

<p align="center">
  <a href="https://github.com/cossacklabs/themis/releases"><img src="https://img.shields.io/github/release/cossacklabs/themis.svg" alt="GitHub release"></a>
  <a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20macOS%20%7C%20Linux%20%7C%20Java%20%7C%20WASM-green.svg" alt="Platforms"></a>
  <a href='https://coveralls.io/github/cossacklabs/themis'><img src='https://coveralls.io/repos/github/cossacklabs/themis/badge.svg?branch=master' alt='Coverage Status' /></a>
  <a href='https://goreportcard.com/report/github.com/cossacklabs/themis'><img class="badge" tag="github.com/cossacklabs/themis" src="https://goreportcard.com/badge/github.com/cossacklabs/themis"></a>
  <br>
  <a href="https://github.com/cossacklabs/themis/actions?query=workflow%3A%22Themis+Core%22"><img src="https://github.com/cossacklabs/themis/workflows/Themis%20Core/badge.svg" alt="Themis Core"></a>
  <a href="https://github.com/cossacklabs/themis/actions?query=workflow%3A%22Integration+testing%22"><img src="https://github.com/cossacklabs/themis/workflows/Integration%20testing/badge.svg" alt="Integration testing"></a>
  <a href="https://github.com/cossacklabs/themis/actions?query=workflow%3A%22Code+style%22"><img src="https://github.com/cossacklabs/themis/workflows/Code%20style/badge.svg" alt="Code style"></a>
  <a href="https://circleci.com/gh/cossacklabs/themis"><img src="https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield" alt="Circle CI"></a>
  <a href="https://app.bitrise.io/app/69a1d5c9d42fa60f"><img src="https://app.bitrise.io/app/69a1d5c9d42fa60f/status.svg?token=Fp_06Ema5PgzBbZQyQy1bA&branch=master" alt="Bitrise"></a>
</p>
<h4 align="center">General purpose cryptographic library for storage and messaging for iOS (Swift, Obj-C), Android (Java, Kotlin), React Native (iOS, Android), desktop Java, С/С++, Node.js, Python, Ruby, PHP, Go, Rust, WASM.</h4>
<h4 align="center">Perfect fit for multi-platform apps. Hides cryptographic details. Made by cryptographers for developers 🧡</h4>



## What Themis is

Themis is an open-source high-level cryptographic services library for securing data during authentication, storage, messaging, network exchange, etc. Themis solves 90% of typical data protection use cases that are common for most apps. 

Themis helps to build both simple and complex cryptographic features easily, quickly, and securely. Themis allows developers to focus on the main thing: developing their applications.

## Use cases that Themis solves

* **Encrypt stored secrets** in your apps and backend: API keys, session tokens, files.

* **Encrypt sensitive data fields** before storing in database (_"application-side field-level encryption"_).

* Support **searchable encryption**, data tokenization and data masking using Themis and [Acra](https://www.cossacklabs.com/acra/).

* Exchange secrets securely: **share sensitive data** between parties, build simple chat app between patients and doctors.

* Build **end-to-end encryption schemes** with centralised or decentralised architecture: encrypt data locally on one app, use it encrypted everywhere, decrypt only for authenticated user.

* Maintain **real-time secure sessions**: send encrypted messages to control connected devices from your app, receive real-time sensitive data from your apps to your backend.

* **Compare secrets** between parties without revealing them (zero-knowledge proof-based authentication).

* **One cryptographic library that fits them all**: Themis is the best fit for multi-platform apps (e.g., iOS+Android+Electron app with Node.js backend) because it provides 100% compatible API and works in the same way across all supported platforms.


## Cryptosystems

Themis provides ready-made building blocks *([“cryptosystems”](https://docs.cossacklabs.com/themis/#cryptosystems))* which simplify usage of core cryptographic security operations.

Themis provides 4 important cryptographic services:

* **[Secure Cell](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/)**: a multi-mode cryptographic container suitable for **storing anything** from encrypted files to database records and format-preserved strings. Secure Cell is built around AES-256-GCM, AES-256-CTR.
* **[Secure Message](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-message/)**: a simple **encrypted messaging** solution for the widest scope of applications. Exchange the keys between the parties and you're good to go. Two pairs of underlying cryptosystems: ECC + ECDSA / RSA + PSS + PKCS#7.
* **[Secure Session](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-session/)**: **session-oriented encrypted data exchange** with forward secrecy for better security guarantees and more demanding infrastructures. Secure Session can perfectly function as socket encryption, session security, or a high-level messaging primitive (with some additional infrastructure like PKI). ECDH key agreement, ECC & AES encryption.
* **[Secure Comparator](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-comparator/)**: Zero knowledge proofs-based cryptographic protocol for **authentication** and comparing secrets.

We created Themis to build other products on top of it - i.e. [Acra](https://www.cossacklabs.com/acra/) and [Hermes](https://www.cossacklabs.com/hermes/).

# Installation

Refer to the [Installation](https://docs.cossacklabs.com/themis/installation/) page to install Themis for your mobile, web, desktop, or server-side application.
We highly recommend [installation packages](https://docs.cossacklabs.com/themis/installation/installation-from-packages/) instead of building from source.


# Languages

Themis is available for the following languages/platforms, refer to [language howtos](https://docs.cossacklabs.com/themis/getting-started/#language-howtos) for each: 

| Platform | Documentation | Examples | Version |
| :----- | :----- | :------ | :--- |
| ⚛️ React Native (iOS, Android) | [React Native Howto](https://docs.cossacklabs.com/themis/languages/react-native/) | [docs/examples/react-native](https://github.com/cossacklabs/themis/tree/master/docs/examples/react-native) | [![npm](https://img.shields.io/npm/v/react-native-themis.svg)](https://www.npmjs.com/package/react-native-themis) |
| 🔶 Swift (iOS, macOS) | [Swift Howto](https://docs.cossacklabs.com/themis/languages/swift/) | [docs/examples/swift](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) | [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| 📱 Objective-C (iOS, macOS) | [Objective-C Howto](https://docs.cossacklabs.com/themis/languages/objc/) | [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)| [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| ☕️ Java (Desktop) | [Java (Desktop) Howto](https://docs.cossacklabs.com/themis/languages/java/installation-desktop/) | [Java projects](https://github.com/cossacklabs/themis-java-examples) | |
| ☎️ Java (Android) | [Java (Android) Howto](https://docs.cossacklabs.com/themis/languages/java/installation-android/) | [Android projects](https://github.com/cossacklabs/themis-java-examples) | [![maven](https://api.bintray.com/packages/cossacklabs/maven/themis/images/download.svg)](https://bintray.com/cossacklabs/maven/themis/_latestVersion) |
| 📞 Kotlin (Android) | [Java (Android) Howto](https://docs.cossacklabs.com/themis/languages/java/installation-android/) | [Android projects](https://github.com/cossacklabs/themis-java-examples) | [![maven](https://api.bintray.com/packages/cossacklabs/maven/themis/images/download.svg)](https://bintray.com/cossacklabs/maven/themis/_latestVersion) |
| 🔻 Ruby | [Ruby Howto](https://docs.cossacklabs.com/themis/languages/ruby/) | [docs/examples/ruby](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby) | [![Gem](https://img.shields.io/gem/v/rbthemis.svg)](https://rubygems.org/gems/rbthemis) |
| 🐍 Python | [Python Howto](https://docs.cossacklabs.com/themis/languages/python/) | [docs/examples/python](https://github.com/cossacklabs/themis/tree/master/docs/examples/python) | [![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search) |
| 🐘 PHP | [PHP Howto](https://docs.cossacklabs.com/themis/languages/php/) | [docs/examples/php](https://github.com/cossacklabs/themis/tree/master/docs/examples/php) | |
| ➕ C++ | [CPP Howto](https://docs.cossacklabs.com/themis/languages/cpp/) | [docs/examples/c++](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) ||
| 🍭 Node.js | [Javascript (Node.js) Howto](https://docs.cossacklabs.com/themis/languages/nodejs/) | [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis) |
| 🖥 WebAssembly | [Javascript (WebAssembly) Howto](https://docs.cossacklabs.com/themis/languages/wasm/)| [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/wasm-themis.svg)](https://www.npmjs.com/package/wasm-themis) |
| 🐹 Go | [Go Howto](https://docs.cossacklabs.com/themis/languages/go/)| [docs/examples/go](https://github.com/cossacklabs/themis/tree/master/docs/examples/go) | [![go.dev](https://img.shields.io/badge/go.dev-v0.14.0-007d9c)](https://pkg.go.dev/mod/github.com/cossacklabs/themis/gothemis) |
| 🦀 Rust | [Rust Howto](https://docs.cossacklabs.com/themis/languages/rust/)| [docs/examples/rust](https://github.com/cossacklabs/themis/tree/master/docs/examples/rust) | [![crates](https://img.shields.io/crates/v/themis.svg)](https://crates.io/crates/themis)|
| 🕸 С++ PNaCl for Google Chrome||[WebThemis project](https://github.com/cossacklabs/webthemis)|

# Availability

Themis supports following CPU architectures: x86_64/i386, ARM, Apple Silicon (ARM64), various Android architectures.

We build and verify Themis on the latest stable OS versions:

  - Debian (9, 10), CentOS (7, 8), Ubuntu (16.04, 18.04, 20.04)
  - macOS (10.12–10.15, 11.*)
  - Android (7–12)
  - iOS (11–15)
  - Windows (experimental MSYS2 support)

We plan to expand this list with a broader set of platforms.
If you'd like to help improve or bring Themis to your favourite platform or language —
[get in touch](mailto:dev@cossacklabs.com).

# Documentation

[Documentation for Themis](https://docs.cossacklabs.com/themis/) contains the ever-evolving official docs, which covers everything from deployment guidelines to use cases, with brief explanations of cryptosystems and architecture behind the main Themis library. 

Refer to the documentation to learn more about:
- [cryptography in Themis](https://docs.cossacklabs.com/themis/crypto-theory/) ([Themis cryptosystems and attacks on them](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/), [cryptography donors](https://docs.cossacklabs.com/themis/crypto-theory/cryptography-donors/), [key management advice](https://docs.cossacklabs.com/themis/crypto-theory/key-management/), [FIPS 140-2 and GOST](https://docs.cossacklabs.com/themis/crypto-theory/fips-and-gost/)),
- [debugging and troubleshooting Themis](https://docs.cossacklabs.com/themis/debugging/) ([command-line utilities](https://docs.cossacklabs.com/themis/debugging/cli-utilities/), [thread safety](https://docs.cossacklabs.com/themis/debugging/thread-safety/), [migration guidelines between Themis versions](https://docs.cossacklabs.com/themis/debugging/migration-guides/)),  
- [Themis architecture](https://docs.cossacklabs.com/themis/architecture/),
- [Themis security](https://docs.cossacklabs.com/themis/security/) ([Themis and OWASP](https://docs.cossacklabs.com/themis/security/owasp/), [Themis development security practices](https://docs.cossacklabs.com/themis/security/dev-security-practices/)),
- [community behind Themis](https://docs.cossacklabs.com/themis/community/) ([contributing guidelines](https://docs.cossacklabs.com/themis/community/contributing/), [credits and honorable mentions](https://docs.cossacklabs.com/themis/community/credits/),  [projects that use Themis](https://docs.cossacklabs.com/themis/community/projects-that-use-themis/), [tutorials](https://docs.cossacklabs.com/themis/community/tutorials-workshops-talks/), [workshops and talks](https://docs.cossacklabs.com/themis/community/tutorials-workshops-talks/)).

# Cryptography

Themis relies on proven cryptographic algorithms implemented by well-known cryptography libraries such as OpenSSL, LibreSSL, BoringSSL. Refer to [Cryptography in Themis](https://docs.cossacklabs.com/themis/crypto-theory/) docs to learn more.


This distribution includes cryptographic software. The country in which you currently reside may have restrictions on the import, possession, use, and/or re-export to another country, of encryption software. BEFORE using any encryption software, please check your country's laws, regulations, and policies concerning the import, possession, or use, and re-export of encryption software, to see if this is permitted. See http://www.wassenaar.org/ for more information.

The U.S. Government Department of Commerce, Bureau of Industry and Security (BIS), has classified this software as Export Commodity Control Number (ECCN) 5D002.C.1, which includes information security software using or performing cryptographic functions with asymmetric algorithms. The form and manner of this distribution make it eligible for export under the License Exception ENC Technology Software Unrestricted (TSU) exception (see the BIS Export Administration Regulations, Section 740.13) for both object code and source code.

# Submitting apps to the App Store

If your application uses Themis and you want to submit it to the Apple App Store, there are certain requirements towards declaring use of any cryptography.

Read about [Apple export regulations on cryptography for Themis](https://docs.cossacklabs.com/themis/regulations/apple-crypto-regulations/) to find out what to do.

# Security

Each change in Themis core library is being reviewed and approved by our internal team of cryptographers and security engineers. For every release, we perform internal audits by cryptographers who don't work on Themis.

We use a lot of automated security testing, i.e. static code analysers, fuzzing tools, memory analysers, unit tests (per each platform), integration tests (to find compatibility issues between different Themis-supported languages, OS and x86/x64 architectures). Read more about our security testing practices in [Themis security docs](https://docs.cossacklabs.com/themis/security/).

If you believe that you've found a security-related issue, please drop us an email to [dev@cossacklabs.com](mailto:dev@cossacklabs.com). Bug bounty program may apply.

# GDPR, HIPAA, CCPA  

As a cryptographic services library for mobile and server platforms, Themis is a ["state of the art"](https://gdpr-info.eu/art-32-gdpr/) encryption tool, which provides secure data exchange and storage. 

Using Themis, you can reach better compliance with the current data privacy regulations, such as:     
* [General Data Protection Regulation (GDPR)](https://gdpr-info.eu/)   
* [HIPAA (Health Insurance Portability and Accountability Act)](https://en.wikipedia.org/wiki/Health_Insurance_Portability_and_Accountability_Act)        
* [DPA (Data Protection Act)](http://www.legislation.gov.uk/ukpga/2018/12/contents/enacted)     
* [CCPA (California Consumer Privacy Act)](https://en.wikipedia.org/wiki/California_Consumer_Privacy_Act)      

Read more about [Regulations](https://docs.cossacklabs.com/themis/regulations/) in docs.

# Community

Themis is [recommended by OWASP](https://github.com/OWASP/owasp-mstg/blob/1.1.0/Document/0x06e-Testing-Cryptography.md#third-party-libraries) as data encryption library for mobile platforms.


Themis is widely-used for both non-commercial and commercial projects, [some public applications and libraries can be found here](https://docs.cossacklabs.com/themis/community/projects-that-use-themis/).

Want to be featured on our blog and on the list of contributors, too? [Write us](mailto:dev@cossacklabs.com) about the project you’ve created using Themis!

# Contributing

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/themis/issues). Head over to our [Contribution guidelines](https://docs.cossacklabs.com/themis/community/contributing/) as your starting point.

Supporting Themis for all these numerous platforms is hard work, but we try to do our best to make using Themis convenient for everyone. Most issues that our users encounter are connected with the installation process and dependency management. If you face any challenges, please [let us know](https://github.com/cossacklabs/themis/issues).

# Commercial support

At Cossack Labs, we offer professional support services for Themis and applications using Themis. 

This support includes, but is not limited to the library integration, with a focus on web and mobile applications; designing and building end-to-end encryption schemes for mobile applications; security audits, for in-house library integrations or high-level protocol; custom application development that requires cryptography; consulting and [training services](https://training.cossacklabs.com).

Drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com) or check out the [Cossack Labs cybersecurity services](https://www.cossacklabs.com/services/overview/).

# Contacts

If you want to ask a technical question, report a bug or suggest a feature,
feel free to [start a discussion on GitHub](https://github.com/cossacklabs/themis/discussions),
raise an issue in the [issue tracker](https://github.com/cossacklabs/themis/issues),
or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).

[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Dev.to CossackLabs](https://img.shields.io/badge/dev.to-%40cossacklabs-orange.svg)](https://dev.to/@cossacklabs/) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/) 

[![Closed pull requests with Bitcode-related changes](https://img.shields.io/github/issues-pr-closed/cossacklabs/themis/Bitcode%20%F0%9F%90%99?color=informational&label=Bitcode-related%20issues)](https://github.com/cossacklabs/themis/pulls?q=is%3Apr+is%3Aclosed+label%3A%22Bitcode+%F0%9F%90%99%22)

<!--[![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) -->
