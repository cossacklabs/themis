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
  <a href="https://github.com/cossacklabs/themis/tree/master/tests"><img src="https://app.bitrise.io/app/b32b4ea8bffedad7/status.svg?token=sFMwuzxY2zy26LsqMkjuqg" alt="Bitrise"></a>
  <a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20macOS%20%7C%20Linux%20%7C%20Java-green.svg" alt="Platforms"></a>
  <a href='https://coveralls.io/github/cossacklabs/themis'><img src='https://coveralls.io/repos/github/cossacklabs/themis/badge.svg?branch=master' alt='Coverage Status' /></a>
  <a href='https://goreportcard.com/report/github.com/cossacklabs/themis'><img class="badge" tag="github.com/cossacklabs/themis" src="https://goreportcard.com/badge/github.com/cossacklabs/themis"></a>
</p>
<br>

#### Crypto library for storage and messaging for Swift, Obj-C, Android, Desktop Java, С++, Node.js, Python, Ruby, PHP, Go, Rust.
<br>

Themis is an open-source high-level cryptographic services library for mobile and server platforms, which provides secure data exchange and storage.

Themis provides four important cryptographic services:

* **[Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)**: a multi-mode cryptographic container suitable for storing anything from encrypted files to database records and format-preserved strings. Secure Cell is built around AES-256 in GCM (Token and Seal modes) and CTR (Context imprint mode).
* **[Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem)**: a simple encrypted messaging solution for the widest scope of applications. Exchange the keys between the parties and you're good to go. Two pairs of the underlying crytosystems: ECC + ECDSA / RSA + PSS + PKCS#7. 
* **[Secure Session](https://github.com/cossacklabs/themis/wiki/Secure-Session-cryptosystem)**: session-oriented, forward secrecy datagram exchange solution with better security guarantees, but more demanding infrastructure. Secure Session can perfectly function as socket encryption, session security, or (with some additional infrastructure) as a high-level messaging primitive. ECDH key agreement, ECC & AES encryption.
* **[Secure Comparator](https://github.com/cossacklabs/themis/wiki/Secure-Comparator-cryptosystem)**: a Zero-Knowledge based cryptographic protocol for authentication and comparing secrets.

**Want to go straight to the detailed documentation?** [Please proceed here](https://docs.cossacklabs.com/pages/documentation-themis/#start-here). 

We created Themis to build other products on top of it - i.e. [Acra](https://github.com/cossacklabs/acra) and [Hermes](https://github.com/cossacklabs/hermes-core).

Themis works on most operating systems (see [Availability](https://github.com/cossacklabs/themis#availability)) and is available for [Swift (iOS, macOS)](https://docs.cossacklabs.com/pages/swift-howto/), [Objective-C (iOS, macOS)](https://docs.cossacklabs.com/pages/objective-c-howto/), [Java desktop and Android](https://docs.cossacklabs.com/pages/java-and-android-howto/), [Ruby](https://docs.cossacklabs.com/pages/ruby-howto/),  [Python](https://docs.cossacklabs.com/pages/python-howto/), 
[PHP](https://docs.cossacklabs.com/pages/php-howto/), 
[C++](https://docs.cossacklabs.com/pages/cpp-howto/), 
[Node.js](https://docs.cossacklabs.com/pages/nodejs-howto/), [Go](https://docs.cossacklabs.com/pages/go-howto/), [Rust](https://docs.cossacklabs.com/pages/rust-howto/),
[Google Chrome](https://github.com/cossacklabs/webthemis). 

We're also porting Themis features to different environments where people might need them: [Redis module](https://www.github.com/cossacklabs/rd_themis), [PostgreSQL module](https://www.github.com/cossacklabs/pg_themis)

Themis was designed to provide complicated cryptosystems in an easy-to-use infrastructure, with modern rapid development in mind:

* **EASY**: Themis does not require users to obsess over parameters, cipher combination, and yet it provides high levels of security.
* **DO YOUR THING**: Themis allows developers to focus on doing the necessary: developing their applications. 
* **BEST PRACTICE**: Themis is based on the best modern practices in implementing complicated security systems.

Themis relies on [the best available](https://github.com/cossacklabs/themis/wiki/Cryptographic-donors) open-source implementations of cryptographic primitives (ciphers). 

Themis is open source, Apache 2 Licensed. 

# Quickstart

TODO: update links

## Install from package managers

The easiest way to install Themis is [to use package repositories for your OS and language](https://docs.cossacklabs.com/pages/documentation-themis/#getting-themis). Package repositories contain the stable versions of Themis, prebuilt and packaged for the most widely-used systems. Each supported platform has 

Installation for server-side platforms (like Linux, Debian, Ubuntu, macOS) consists of two parts: installing Themis Core library into system and installing Themis language wrapper to use from your application. Refer to [the Installation guide](https://docs.cossacklabs.com/pages/documentation-themis/#getting-themis).

Installation for mobile platforms (iOS, Android) is easier: just use package manager popular at this platform. Refer to [the Installation guide](https://docs.cossacklabs.com/pages/documentation-themis/#getting-themis).

## Install from sources

If you need the latest development version of Themis, or your system is not supported yet, you can [build and install Themis from GitHub source code](https://docs.cossacklabs.com/pages/documentation-themis/#getting-themis).

# Languages

Themis is available for the following languages/platforms: 

| Platform | Documentation | Examples | Version |
| :----- | :----- | :------ | :--- |
| 🔶 Swift (iOS, macOS) | [Swift Howto](https://docs.cossacklabs.com/pages/swift-howto/) | [docs/examples/swift](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) | [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| 📱 Objective-C (iOS, macOS) | [Objective-C Howto](https://docs.cossacklabs.com/pages/objective-c-howto/) | [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)| [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| ☕️ Java Desktop and Android | [Java & Android Howto](https://docs.cossacklabs.com/pages/java-and-android-howto/) | [Java and Android projects](https://github.com/cossacklabs/themis-java-examples) ||
| ♦️ Ruby | [Ruby Howto](https://docs.cossacklabs.com/pages/ruby-howto/) | [docs/examples/ruby](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby) | [![Gem](https://img.shields.io/gem/v/rbthemis.svg)](https://rubygems.org/gems/rbthemis) |
| 🐍 Python | [Python Howto](https://docs.cossacklabs.com/pages/python-howto/) | [docs/examples/python](https://github.com/cossacklabs/themis/tree/master/docs/examples/python) | [![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search) |
| 🐘 PHP | [PHP Howto](https://docs.cossacklabs.com/pages/php-howto/) | [docs/examples/php](https://github.com/cossacklabs/themis/tree/master/docs/examples/php) | |
| ➕ C++ | [CPP Howto](https://docs.cossacklabs.com/pages/cpp-howto/) | [docs/examples/c++](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) ||
| 🍭 Node.js | [Node.js Howto](https://docs.cossacklabs.com/pages/nodejs-howto/) | [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis) |
| 🐹 Go | [Go Howto](https://docs.cossacklabs.com/pages/go-howto/)| [docs/examples/go](https://github.com/cossacklabs/themis/tree/master/docs/examples/go) ||
| ☢️ Rust | [Rust Howto](https://docs.cossacklabs.com/pages/rust-howto/)| [docs/examples/rust](https://github.com/cossacklabs/themis/tree/master/docs/examples/rust) | [![crates](https://img.shields.io/crates/v/themis.svg)](https://crates.io/crates/themis)|
| 🕸 С++ PNaCl for Google Chrome||[WebThemis project](https://github.com/cossacklabs/webthemis)|


# Plugins

Themis-based plugins are built to enable Themis' features across various platforms and products: 

- [RD_Themis](https://github.com/cossacklabs/rd_themis) for Redis database
- [PG_Themis](https://github.com/cossacklabs/pg_themis) for PostgreSQL database

# Availability

Themis supports the following architectures: x86/x64, armv*, various Android architectures.

It is checked to compile on the latest stable versions of:

* Debian (8, 9), CentOS 7, Ubuntu (14.04, 16.04, 18.04),
* macOS (10.12, 10.13, 10.14),
* Android (4 - 9) / CyanogenMod 11+,
* iOS (9 - 12).

We plan to expand this list with broader set of platforms. If you'd like to help improve or bring Themis to your favourite platform / language — [get in touch](dev@cossacklabs.com).

# Tutorials

As long as it remains feasible, we'll be accumulating the list of all our tutorials on how to use Themis in different cases here:

* [Building end-to-end encrypted Firebase-based application](https://github.com/vixentael/zka-example) for note sharing: build bullet-proof application even when using backend-as-a-service which can't be fully trusted.

* [Key management basics for iOS](https://github.com/vixentael/ios-datasec-basics) shows numerous ways how to store keys (API tokens, user passwords) using obfuscation and encryption techniques.

* [Releasing Themis into public: usability testing](https://www.cossacklabs.com/02-usability-testing.html), which goes a bit into how to use Secure Message for iOS and Python. Go directly into [corresponding GitHub repository](https://github.com/cossacklabs/themis-ux-testing) to play with code. 

* [Building encrypted chat service with Themis and mobile websocket example](https://www.cossacklabs.com/building-secure-chat), which outlines stages necessary to build encrypted chat service around Ruby websocket server with clients in iOS and Android. [GitHub repository](https://github.com/cossacklabs/mobile-websocket-example) with accompanying code.

# Sample projects

During the development stage we frequently do Proof-of-Concept projects to test different assumptions. They serve as interesting demos of what Themis is capable of:

| Demo | Description | Repo | Blog post |
| :----- | :----- | :------ | :--- |
| 0fc | Anonymous web chat<br/>* Python<br/>* webthemis (C++ + HTML/JS)| [repo](https://github.com/cossacklabs/0fc) | [blog post](https://cossacklabs.com/building-endtoend-webchat.html) |
| Sesto | Secure storage<br/>* Python<br/>* webthemis (C++ + HTML/JS) | [repo](https://github.com/cossacklabs/sesto) |[blog post](https://cossacklabs.com/presenting-sesto.html) 
| Swift Alps demo | Secure communication (iOS app with Python server based on Secure Session)<br/>* Swift<br/>* Python | [repo](https://github.com/cossacklabs/theswiftalpsdemo) |[slides](https://speakerdeck.com/vixentael/the-swift-alps-security-workshop) |
| Zero-Knowledge<br/> Architectures<br/>workshop | iOS app for storing and sharing encrypted notes stored in Firebase database<br/>* Swift | [repo](https://github.com/vixentael/zka-example) | |
| Java and<br/>Android examples | A set of handy examples that show how to encrypt data for storing (Themis Secure Cell) or for sending to peers (Themis Secure Message and Themis Secure Session)<br/>* Java | [repo](https://github.com/cossacklabs/themis-java-examples) | |

# Themis Server

If you'd like to experiment with Themis in a more interactive environment, check out [Themis Server](https://themis.cossacklabs.com), interactive debugging environment for Themis. Themis Server can verify and decrypt code encrypted by Secure Cell or Secure Message, and provides a fully-functional backend for Secure Session. Examples for many languages are available in [docs/examples/Themis-server](https://github.com/cossacklabs/themis/tree/master/docs/examples/Themis-server).

# Community

TODO: add link to blog post with Themis projects


# Contributing to us

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/themis/issues). Head over to our [Contribution guidelines](https://github.com/cossacklabs/themis/wiki/Contribute) as your starting point.


# Documentation

[Project's DocServer](https://docs.cossacklabs.com/products/themis/) contains the ever-evolving official documentation, which contains everything from deployment guidelines to use-cases, with a brief explanation of cryptosystems and architecture behind the main Themis library sandwiched in. 


# Contacts

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/themis/issues) or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).
   
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/) [![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

