<h3 align="center">
  <a href="https://www.cossacklabs.com"><img src="https://github.com/cossacklabs/themis/wiki/images/logo.png" alt="Themis provides strong, usable cryptography for busy people" width="300"></a>
  <br>
  <br>
  Themis provides strong, usable cryptography for busy people
  <br>
  <br>
</h3>

---

<p align="center">
	<a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/github/release/cossacklabs/themis.svg" alt="GitHub release"></a>
  <a href="https://circleci.com/gh/cossacklabs/themis"><img src="https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield" alt="Circle CI"></a>
   <a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20macOS%20%7C%20Linux%20%7C%20Windows%20%7C%20Java-green.svg" alt="Platforms"></a>
</p>
<br>

#### Crypto library for storage and messaging for Swift, ObjC, Android, С++, JS, Python, Ruby, PHP, Go.
<br>

Themis is open-source high-level cryptographic services library for mobile and server platforms, providing secure data exchange and  storage. Current stable release is [0.9.4](https://github.com/cossacklabs/themis/releases/tag/0.9.4), dated 22nd of November.

Themis provides four important cryptographic services:

* **[Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem)**: a simple encrypted messaging solution for widest scope of applications. Just exchange the keys between parties and you're good to go! Two pairs of underlying crytosystems: ECC + ECDSA / RSA + PSS + PKCS#7. 
* **[Secure Session](https://github.com/cossacklabs/themis/wiki/Secure-Session-cryptosystem)**: session-oriented, forward secrecy datagram exchange solution with better security guarantees, but more demanding infrastructure. Secure Session works perfect as socket encryption, session security or (with some additional infrastructure) as high-level messaging primitive. ECDH key agreement, ECC & AES encryption.
* **[Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)**: a multi-mode cryptographic container, suitable for storing anything from encrypted files to database records and format-preserved strings. Secure Cell is built around AES in GCM (Token and Seal modes) and CTR (Context imprint mode).
* **[Secure Comparator](https://github.com/cossacklabs/themis/wiki/Secure-Comparator-cryptosystem)**: a Zero-Knowledge based cryptographic protocol for authentication and comparing secrets.

Themis works in most operating systems (see [Availability](https://github.com/cossacklabs/themis#availability)), and is available for [Swift (iOS, macOS)](https://github.com/cossacklabs/themis/wiki/Swift-Howto), [Objective-C (iOS, macOS)](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto), [Java+Android](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto),  [Ruby](https://github.com/cossacklabs/themis/wiki/Ruby-Howto),  [Python](https://github.com/cossacklabs/themis/wiki/Python-Howto), 
[PHP](https://github.com/cossacklabs/themis/wiki/PHP-Howto), 
[C++](https://github.com/cossacklabs/themis/wiki/CPP-Howto), 
[Javascript (NodeJS)](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto), [Go](https://github.com/cossacklabs/themis/wiki/Go-HowTo),
[Google Chrome](https://github.com/cossacklabs/webthemis). 

We're also porting Themis features to different environments where people might need them: [Redis module](https://www.github.com/cossacklabs/rd_themis), [PostgreSQL module](https://www.github.com/cossacklabs/pg_themis)

Themis was designed to provide complicated cryptosystems in easy-to-use infrastructure, designed with modern rapid development in mind:

* **EASY**: Themis does not require users to obsess over parameters, cipher combination and yet achieve high levels of security
* **DO YOUR THING**: Themis allows developers to focus on doing what is necessary: developing their applications. 
* **BEST PRACTICE**: Themis is based on best modern practices in implementing complicated security systems.

Themis is open source, Apache 2 Licensed. 

# Quickstart

1. Fetch the repository: git clone https://github.com/cossacklabs/themis.git
2. Have OpenSSL/LibreSSL + OpenSSL/LibreSSL Dev package (libssl-dev) installed at typical paths: `/usr/lib`, `/usr/include`. 
3. Have typical GCC/clang environment installed
4. Type 'make install' and you're done (most of the cases)
5. Dive into [our wiki](https://github.com/cossacklabs/themis/wiki) for the docs of language of your choice, take a look at docs/examples for examples. 

It is really advisable to [go the long way and read the docs](https://github.com/cossacklabs/themis/wiki/3.1-Building-and-installing), but god blesses the brave.

# Languages

Themis is available for the following languages/platforms: 

| Platform | Documentation | Examples | Version |
| :----- | :----- | :------ | :--- |
| 🔶 Swift (iOS, OSX) | [Swift Howto](https://github.com/cossacklabs/themis/wiki/Swift-Howto) | [docs/examples/swift](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) | [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| 📱 Objective-C (iOS, OSX) | [Objective-C Howto](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto) | [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)| [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| ☕️ Java / Android | [Java & Android Howto](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto) | [jni example](https://github.com/cossacklabs/themis/tree/master/jni) ||
| ♦️ Ruby | [Ruby Howto](https://github.com/cossacklabs/themis/wiki/Ruby-Howto) | [docs/examples/ruby](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby) | [![Gem](https://img.shields.io/gem/v/rubythemis.svg)](https://rubygems.org/gems/rubythemis) |
| 🐍 Python | [Python Howto)](https://github.com/cossacklabs/themis/wiki/Python-Howto) | [docs/examples/python](https://github.com/cossacklabs/themis/tree/master/docs/examples/python) | [![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search) |
| 🐘 PHP | [PHP Howto](https://github.com/cossacklabs/themis/wiki/PHP-Howto) | [docs/examples/php](https://github.com/cossacklabs/themis/tree/master/docs/examples/php) | |
| ➕ C++ | [CPP Howto)](https://github.com/cossacklabs/themis/wiki/CPP-Howto) | [docs/examples/c++](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) ||
| 🍭 Javascript (NodeJS) | [NodeJS Howto](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto) | [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis) |
| 🐨 Go | [Go-Howto](https://github.com/cossacklabs/themis/wiki/Go-Howto)| [docs/examples/go](https://github.com/cossacklabs/themis/tree/master/docs/examples/go) ||
| 🕸 С++ PNaCl for Google Chrome||[WebThemis project](https://github.com/cossacklabs/webthemis)|


# Plugins

Themis-based plugins are built to enable Themis features across various platforms and products: 

- [RD_Themis](https://github.com/cossacklabs/rd_themis) for Redis database
- [PG_Themis](https://github.com/cossacklabs/pg_themis) for PostgreSQL database


# Availability

Themis supports the following architectures: x86/x64, armv*, various androids

It is checked to compile on latest stable versions of :

* Debian 7.8+, CentOS 7, Ubuntu 14, Arch Linux 2014+
* Windows XP+
* OSX 10.9+
* Android 4-5+ / CyanogenMod 11+
* iOS8—iOS10+, x32/x64

We plan to expand this minuscule availability scope with broader set of platforms. If you'd like to help Themis arrive (or get better) on your favourite platform / language — get in touch.

# Tutorials

As long as it is feasible, we'll accumulate list of all tutorials we publish on how to use Themis in different cases here:

* [Releasing Themis into public: usability testing](https://www.cossacklabs.com/02-usability-testing.html), which goes a bit into how to use Secure Message for iOS and Python. Go directly into [corresponding github repository](https://github.com/cossacklabs/themis-ux-testing) to play with code. 
* [Building encrypted chat service with Themis and mobile websocket example](https://www.cossacklabs.com/building-secure-chat), which outlines stages necessary to build encrypted chat service around Ruby websocket server, with clients in iOS and Android. [Github repository](https://github.com/cossacklabs/mobile-websocket-example) with code for the post.

# Sample projects

During development, we frequently do Proof-of-Concept projects to test different assumptions. They serve as interesting demos of what Themis is capable of:

| Demo | Description | Repo | Blog post |
| :----- | :----- | :------ | :--- |
| 0fc | Anonymous web chat<br/>* pythemis (Python)<br/>* webthemis (C++ + HTML/JS)| [repo](https://github.com/cossacklabs/0fc) | [blog post](https://cossacklabs.com/building-endtoend-webchat.html) |
| sesto | Secure storage<br/>* pythemis (Python)<br/>* webthemis (C++ + HTML/JS) | [repo](https://github.com/cossacklabs/sesto) |[blog post](https://cossacklabs.com/presenting-sesto.html) 
| swift alps demo | Secure communication (iOS app with Python server based on Secure Session)<br/>* Swift wrapper<br/>* pythemis (Python) | [repo](https://github.com/cossacklabs/theswiftalpsdemo) |[slides](https://speakerdeck.com/vixentael/the-swift-alps-security-workshop) |

# Themis Server

If you'd like to experiment with Themis in more interactive environment, check out [Themis Server](https://themis.cossacklabs.com), interactive debugging environment for Themis. 

# Documentation

[Project's github wiki](https://www.github.com/cossacklabs/themis/wiki) contains ever-evolving official documentation, which contains everything from how to use it to ways to contribute to it, with a brief explanation of cryptosystems and architecture behind main Themis library in between. 


# Communication

[![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs)
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/)
[![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

