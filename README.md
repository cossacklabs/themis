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
  <a href="https://circleci.com/gh/cossacklabs/themis"><img src="https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield" alt="Circle CI"></a>
  <a href="https://app.bitrise.io/app/69a1d5c9d42fa60f"><img src="https://app.bitrise.io/app/69a1d5c9d42fa60f/status.svg?token=Fp_06Ema5PgzBbZQyQy1bA&branch=master" alt="Bitrise"></a>
  <a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20macOS%20%7C%20Linux%20%7C%20Java%20%7C%20WASM-green.svg" alt="Platforms"></a>
  <a href='https://coveralls.io/github/cossacklabs/themis'><img src='https://coveralls.io/repos/github/cossacklabs/themis/badge.svg?branch=master' alt='Coverage Status' /></a>
  <a href='https://goreportcard.com/report/github.com/cossacklabs/themis'><img class="badge" tag="github.com/cossacklabs/themis" src="https://goreportcard.com/badge/github.com/cossacklabs/themis"></a>
</p>

<h4 align="center">General purpose cryptographic library for storage and messaging for iOS (Swift, Obj-C), Android, desktop Java, С++, Node.js, Python, Ruby, PHP, Go, Rust, WASM.</h4>
<h4 align="center">Perfect fit for multi-platform apps. Hides cryptographic details. Made by cryptographers for developers 🧡</h4>

### Crypto systems

Themis is an open-source high-level cryptographic services library for mobile and server platforms that provides secure data exchange, authentication, and storage protection. Themis provides ready-made building components, which simplify usage of core cryptographic security operations.

Themis provides 4 important cryptographic services:

* **[Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)**: a multi-mode cryptographic container suitable for **storing anything** from encrypted files to database records and format-preserved strings. Secure Cell is built around AES-256 in GCM (Token and Seal modes) and CTR (Context imprint mode).
* **[Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem)**: a simple **encrypted messaging** solution for the widest scope of applications. Exchange the keys between the parties and you're good to go. Two pairs of underlying cryptosystems: ECC + ECDSA / RSA + PSS + PKCS#7. 
* **[Secure Session](https://github.com/cossacklabs/themis/wiki/Secure-Session-cryptosystem)**: **session-oriented encrypted data exchange** with forward secrecy for better security guarantees and more demanding infrastructures. Secure Session can perfectly function as socket encryption, session security, or a high-level messaging primitive (with some additional infrastructure like PKI). ECDH key agreement, ECC & AES encryption.
* **[Secure Comparator](https://github.com/cossacklabs/themis/wiki/Secure-Comparator-cryptosystem)**: Zero knowledge-based cryptographic protocol for **authentication** and comparing secrets.

We created Themis to build other products on top of it - i.e. [Acra](https://github.com/cossacklabs/acra) and [Hermes](https://github.com/cossacklabs/hermes-core).

### Easy to use, hard to misuse 

Implementing cryptography in applications is often hard. Choosing cipher suites, defining key lengths, and designing key exchange schemes require plenty of particular competencies and lead to mistakes when done by applied developers.

Themis was designed to provide complicated cryptosystems in an easy-to-use infrastructure, with modern rapid development in mind:

* **EASY**: Themis does not require users to obsess over parameters, cipher combination, IV, salt, yet it provides high levels of security.
* **DO YOUR THING**: Themis allows developers to focus on the main thing: developing their applications. 
* **BEST PRACTICE**: Themis is based on the best modern practices in implementing complicated security systems.

Themis relies on [the best available](https://github.com/cossacklabs/themis/wiki/Cryptographic-donors) open-source implementations of cryptographic primitives (ciphers).

# Quickstart

## Install from package managers

The easiest way to install Themis is [to use package repositories for your OS and language](https://github.com/cossacklabs/themis/wiki/Installing-Themis). Package repositories contain stable versions of Themis, prebuilt and packaged for the most widely-used systems.     

Installation for server-side platforms (like Debian, Ubuntu, CentOS, macOS) consists of two parts: installing Themis Core library into the system and installing Themis language wrapper to use from your application. Refer to [the Installation guide](https://github.com/cossacklabs/themis/wiki/Installing-Themis#installing-themis-wrappers).

Installation for mobile platforms (iOS, Android) and WebAssembly is easier: just use package manager popular on this platform. Refer to [the Installation guide](https://github.com/cossacklabs/themis/wiki/Installing-Themis#installing-themis-wrappers).

## Install from sources

If you need the latest development version of Themis or your system is not supported yet, you can [build and install Themis from GitHub source code](https://github.com/cossacklabs/themis/wiki/Building-and-Installing).

# Languages

Themis is available for the following languages/platforms: 

| Platform | Documentation | Examples | Version |
| :----- | :----- | :------ | :--- |
| 🔶 Swift (iOS, macOS) | [Swift Howto](https://github.com/cossacklabs/themis/wiki/Swift-Howto) | [docs/examples/swift](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) | [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| 📱 Objective-C (iOS, macOS) | [Objective-C Howto](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto) | [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)| [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| ☕️ Java (Desktop) | [Java & Android Howto](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto) | [Java projects](https://github.com/cossacklabs/themis-java-examples) | |
| ☎️ Java (Android) | [Java & Android Howto](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto) | [Android projects](https://github.com/cossacklabs/themis-java-examples) | [![maven](https://api.bintray.com/packages/cossacklabs/maven/themis/images/download.svg)](https://bintray.com/cossacklabs/maven/themis/_latestVersion) |
| ♦️ Ruby | [Ruby Howto](https://github.com/cossacklabs/themis/wiki/Ruby-Howto) | [docs/examples/ruby](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby) | [![Gem](https://img.shields.io/gem/v/rbthemis.svg)](https://rubygems.org/gems/rbthemis) |
| 🐍 Python | [Python Howto](https://github.com/cossacklabs/themis/wiki/Python-Howto) | [docs/examples/python](https://github.com/cossacklabs/themis/tree/master/docs/examples/python) | [![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search) |
| 🐘 PHP | [PHP Howto](https://github.com/cossacklabs/themis/wiki/PHP-Howto) | [docs/examples/php](https://github.com/cossacklabs/themis/tree/master/docs/examples/php) | |
| ➕ C++ | [CPP Howto](https://github.com/cossacklabs/themis/wiki/CPP-Howto) | [docs/examples/c++](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) ||
| 🍭 Node.js | [Node.js Howto](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto) | [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis) |
| 🖥 WebAssembly | [WASM Howto](https://docs.cossacklabs.com/pages/js-wasm-howto/)| [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/wasm-themis.svg)](https://www.npmjs.com/package/wasm-themis) |
| 🐹 Go | [Go Howto](https://github.com/cossacklabs/themis/wiki/Go-HowTo)| [docs/examples/go](https://github.com/cossacklabs/themis/tree/master/docs/examples/go) ||
| 🦀 Rust | [Rust Howto](https://github.com/cossacklabs/themis/wiki/Rust-Howto)| [docs/examples/rust](https://github.com/cossacklabs/themis/wiki/Rust-Howto) | [![crates](https://img.shields.io/crates/v/themis.svg)](https://crates.io/crates/themis)|
| 🕸 С++ PNaCl for Google Chrome||[WebThemis project](https://github.com/cossacklabs/webthemis)|

Want to jump straight to the documentation? [Please head over here](https://docs.cossacklabs.com/pages/documentation-themis/#start-here).

# Plugins

Themis-based plugins are built to enable Themis' features across various platforms and products: 

- [RD_Themis](https://github.com/cossacklabs/rd_themis) for Redis database,
- [PG_Themis](https://github.com/cossacklabs/pg_themis) for PostgreSQL database.

# Availability

Themis supports the following architectures: x86/x64, armv*, various Android architectures.

It is checked to compile on the latest stable versions of:

* Debian (8, 9), CentOS 7, Ubuntu (16.04, 18.04),
* macOS (10.12 – 10.15),
* Android (4 - 10) / CyanogenMod 11+,
* iOS (9 - 13),
* Windows (MSYS2, experimental feature).

We plan to expand this list with a broader set of platforms. If you'd like to help improve or bring Themis to your favourite platform / language — [get in touch](dev@cossacklabs.com).

# Tutorials

As long as it remains feasible, we'll be accumulating the list of all our tutorials on how to use Themis in different cases here:

* [Building end-to-end encrypted notes in Bear app](https://www.cossacklabs.com/blog/end-to-end-encryption-in-bear-app.html): real-world story on helping Bear.app to implement note encryption for their vast existing user base.

* [Building end-to-end encrypted Firebase-based application](https://github.com/vixentael/zka-example) for note sharing: build a bullet-proof application for when using backend-as-a-service which can't be fully trusted.

* [Key management basics for iOS](https://github.com/vixentael/ios-datasec-basics) illustrates numerous ways to store keys (API tokens, user passwords) using obfuscation and encryption techniques.

* [Releasing Themis into public: usability testing](https://www.cossacklabs.com/02-usability-testing.html) which provides some explanation of how to use Secure Message for iOS and Python. Go directly into [corresponding GitHub repository](https://github.com/cossacklabs/themis-ux-testing) to play with code. 

* [Building encrypted chat service with Themis and mobile websocket example](https://www.cossacklabs.com/building-secure-chat), which outlines the stages necessary for building an encrypted chat service around Ruby websocket server with clients in iOS and Android. [GitHub repository](https://github.com/cossacklabs/mobile-websocket-example) with the accompanying code.

# Sample projects

During the development stage, we frequently do Proof-of-Concept projects to test different assumptions. They serve as interesting demos (examples) of what Themis is capable of:

| Demo | Description | Repo | Blog post |
| :----- | :----- | :------ | :--- |
| 0fc | Anonymous web chat<br/>* Python<br/>* webthemis (C++ + HTML/JS)| [repo](https://github.com/cossacklabs/0fc) | [blog post](https://cossacklabs.com/building-endtoend-webchat.html) |
| Sesto | Secure storage<br/>* Python<br/>* webthemis (C++ + HTML/JS) | [repo](https://github.com/cossacklabs/sesto) |[blog post](https://cossacklabs.com/presenting-sesto.html) 
| Swift Alps demo | Secure communication (iOS app with Python server based on Secure Session)<br/>* Swift<br/>* Python | [repo](https://github.com/cossacklabs/theswiftalpsdemo) |[slides](https://speakerdeck.com/vixentael/the-swift-alps-security-workshop) |
| Zero-Knowledge<br/> Architectures<br/>workshop | iOS app for storing and sharing encrypted notes stored in Firebase database<br/>* Swift | [repo](https://github.com/vixentael/zka-example) | |
| Java and<br/>Android examples | A set of handy examples that show how to encrypt data for storing (Themis Secure Cell) or for sending it to peers (Themis Secure Message and Themis Secure Session)<br/>* Java | [repo](https://github.com/cossacklabs/themis-java-examples) | |
| Android secure storage<br/> library | SecureStorage is used for keeping private information in a safe mode without requiring a password or a fingerprint <br/>* Java<br/>* Kotlin | [repo](https://github.com/epam/Android-Secure-Storage) | |

# Themis Server

If you'd like to experiment with Themis in a more interactive environment, check out [Themis Server](https://themis.cossacklabs.com) interactive debugging environment for Themis. Themis Server can verify and decrypt the code encrypted by Secure Cell or Secure Message and provides a fully-functional backend for Secure Session. Examples for many languages are available in [docs/examples/Themis-server](https://github.com/cossacklabs/themis/tree/master/docs/examples/Themis-server).

# Documentation

[Documentation Server for Themis](https://docs.cossacklabs.com/products/themis/) contains the ever-evolving official documentation, which covers everything from deployment guidelines to use cases, with brief explanations of cryptosystems and architecture behind the main Themis library. 

# Cryptography

This distribution includes cryptographic software. The country in which you currently reside may have restrictions on the import, possession, use, and/or re-export to another country, of encryption software. BEFORE using any encryption software, please check your country's laws, regulations, and policies concerning the import, possession, or use, and re-export of encryption software, to see if this is permitted. See http://www.wassenaar.org/ for more information.

The U.S. Government Department of Commerce, Bureau of Industry and Security (BIS), has classified this software as Export Commodity Control Number (ECCN) 5D002.C.1, which includes information security software using or performing cryptographic functions with asymmetric algorithms. The form and manner of this distribution make it eligible for export under the License Exception ENC Technology Software Unrestricted (TSU) exception (see the BIS Export Administration Regulations, Section 740.13) for both object code and source code.

# Submitting apps to the App Store

If you’re using Themis as your means of encryption within your iOS/macOS app that you’re submitting to the App Store, your encryption falls under the “open source” exception (although if your app is not open source/distributed free of charge, we strongly recommend that you seek legal advice). 

Themis is a free cryptographic library that builds on the existing, community-tested cryptographic instruments (OpenSSL, LibreSSL, BoringSSL, depending on the target platform). It is open source and Apache 2-licensed, with its full source code publicly available online on GitHub. 

This means that you should indicate that you’re using encryption and only submit annual self-classification reports (use [this handy table](https://help.apple.com/app-store-connect/#/devc3f64248f) to self-check). Read more about [Apple regulations on cryptography](https://medium.com/@cossacklabs/apple-export-regulations-on-crypto-6306380682e1) and [check Apple docs](https://developer.apple.com/documentation/security/complying_with_encryption_export_regulations?language=objc).

# Security

Each change in Themis core library is being reviewed and approved by our internal team of cryptographers and security engineers. For every release, we perform internal audits by cryptographers who don't work on Themis.

We use a lot of automated security testing, i.e. static code analysers, fuzzing tools, memory analysers, unit tests (per each platform), integration tests (to find compatibility issues between different Themis-supported languages, OS and x86/x64 architectures). Read more about our security testing practices in [a blog post](https://medium.com/@cossacklabs/automated-security-testing-56ee1253c1fd).

If you believe that you've found a security-related issue, please drop us an email to [dev@cossacklabs.com](mailto:dev@cossacklabs.com). Bug bounty program may apply.

# Community

Themis is [recommended by OWASP](https://github.com/OWASP/owasp-mstg/blob/1.1.0/Document/0x06e-Testing-Cryptography.md#third-party-libraries) as data encryption library for mobile platforms.

<!-- TODO: re-link into internal "Projects the use Themis" page instead of blog post -->

Themis is widely-used for both non-commercial and commercial projects, [some public projects can be found here](https://www.cossacklabs.com/blog/themis-contributors-and-projects-2018.html). [Android secure storage library by EPAM](https://github.com/epam/Android-Secure-Storage) is using Themis under the hood.

Want to be featured on our blog and on the list of contributors, too? [Write us](mailto:dev@cossacklabs.com) about the project you’ve created using Themis!

# Contributing to us

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/themis/issues). Head over to our [Contribution guidelines](https://github.com/cossacklabs/themis/wiki/Contribute) as your starting point.

Supporting Themis for all these numerous platforms is hard work, but we try to do our best to make using Themis convenient for everyone. Most issues that our users encounter are connected with the installation process and dependency management. If you face any challenges, please [let us know](https://github.com/cossacklabs/themis/issues).

# Commercial support

At Cossack Labs, we offer professional support services for Themis and applications using Themis. 

This support includes, but is not limited to the library integration, with a focus on web and mobile applications; designing and building end-to-end encryption schemes for mobile applications; security audits, for in-house library integrations or high-level protocol; custom application development that requires cryptography; consulting and [training services](https://training.cossacklabs.com).

Drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com) or check out the [DataGuardian assistance program](https://cossacklabs.com/dgap/).

# Contacts

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/themis/issues) or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).
   
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/) [![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

