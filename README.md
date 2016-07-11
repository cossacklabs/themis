![](https://github.com/cossacklabs/themis/wiki/images/logo.png)

[![GitHub release](https://img.shields.io/github/release/cossacklabs/themis.svg)](https://github.com/cossacklabs/themis/releases/latest)
[![Circle CI](https://circleci.com/gh/cossacklabs/themis/tree/master.svg?style=shield)](https://circleci.com/gh/cossacklabs/themis)
[![CocoaPods](https://img.shields.io/cocoapods/l/themis.svg)](https://github.com/cossacklabs/themis/blob/master/LICENSE)
![Platforms](https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20OS%20X%20%7C%20Linux%20%7C%20Windows%7C%20Java-green.svg)

[![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis)
[![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search)
[![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis)
[![Gem](https://img.shields.io/gem/v/rubythemis.svg)](https://rubygems.org/gems/rubythemis)

[![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs)
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/)
[![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

#### Themis provides strong, usable cryptography for busy people

Themis is open-source high-level cryptographic services library for mobile and server platforms, providing secure data exchange and  storage. Current stable release is [0.9.3](https://github.com/cossacklabs/themis/releases/tag/0.9.3), dated 24th of May.

Themis works in most operating systems (see [Availability](https://github.com/cossacklabs/themis#availability)), and is available for [Swift (iOS, OX)](https://github.com/cossacklabs/themis/wiki/Swift-Howto), [Objective-C (iOS, OX)](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto), [Java+Android](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto),  [Ruby](https://github.com/cossacklabs/themis/wiki/Ruby-Howto),  [Python](https://github.com/cossacklabs/themis/wiki/Python-Howto), 
[PHP](https://github.com/cossacklabs/themis/wiki/PHP-Howto), 
[C++](https://github.com/cossacklabs/themis/wiki/CPP-Howto), 
[Javascript (NodeJS)](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto), [Go (NodeJS)](https://github.com/cossacklabs/themis/wiki/Go-HowTo)
[Google Chrome](https://github.com/cossacklabs/webthemis). 

Themis provides three important cryptographic services:

* **[Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem)**: a simple encrypted messaging solution for widest scope of applications. Just exchange the keys between parties and you're good to go! Two pairs of underlying crytosystems: ECC + ECDSA / RSA + PSS + PKCS#7. 
* **[Secure Session](https://github.com/cossacklabs/themis/wiki/Secure-Session-cryptosystem)**: session-oriented, forward secrecy datagram exchange solution with better security guarantees, but more demanding infrastructure. Secure Session works perfect as socket encryption, session security or (with some additional infrastructure) as high-level messaging primitive. ECDH key agreement, ECC & AES encryption.
* **[Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)**: a multi-mode cryptographic container, suitable for storing anything from encrypted files to database records and format-preserved strings. Secure Cell is built around AES in GCM (Token and Seal modes) and CTR (Context imprint mode).

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

Themis is available for the following languages: 

- Swift (iOS, OSX) [documentation](https://github.com/cossacklabs/themis/wiki/Swift-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift)
- Objective-C (iOS, OSX) [documentation](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)
- Java / Android [documentation](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto)
- Ruby [documentation](https://github.com/cossacklabs/themis/wiki/Ruby-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby)
- Python [documentation](https://github.com/cossacklabs/themis/wiki/Python-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/python)
- PHP [documentation](https://github.com/cossacklabs/themis/wiki/PHP-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/php)
- C++ [documentation](https://github.com/cossacklabs/themis/wiki/CPP-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B)
- Javascript (NodeJS) [documentation](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto) and [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/js)
- Go [documentation](https://github.com/cossacklabs/themis/wiki/Go-Howto)
- С++ PNaCl for Google Chrome in separate [WebThemis project](https://github.com/cossacklabs/webthemis)

# Availability

Themis supports the following architectures: x86/x64, armv*, various androids

It is checked to compile on latest stable versions of :

* Debian 7.8+, CentOS 7, Ubuntu 14, Arch Linux 2014+
* Windows XP+
* OSX 10.9+
* Android 4-5+ / CyanogenMod 11+
* iOS7—iOS9+, x32/x64

We plan to expand this minuscule availability scope with broader set of platforms. If you'd like to help Themis arrive (or get better) on your favourite platform / language — get in touch.

# Tutorials

As long as it is feasible, we'll accumulate list of all tutorials we publish on how to use Themis in different cases here:

* [Releasing Themis into public: usability testing](https://www.cossacklabs.com/02-usability-testing.html), which goes a bit into how to use Secure Message for iOS and Python. Go directly into [corresponding github repository](https://github.com/cossacklabs/themis-ux-testing) to play with code. 
* [Building encrypted chat service with Themis and mobile websocket example](https://www.cossacklabs.com/building-secure-chat), which outlines stages necessary to build encrypted chat service around Ruby websocket server, with clients in iOS and Android. [Github repository](https://github.com/cossacklabs/mobile-websocket-example) with code for the post.

# Sample projects

During development, we frequently do Proof-of-Concept projects to test different assumptions. They serve as interesting demos of what Themis is capable of:

* 0fc anonymous web chat, pythemis (Python) + webthemis (C++ + HTML/JS): [github repo](https://github.com/cossacklabs/0fc) [blog post](https://cossacklabs.com/building-endtoend-webchat.html)
* sesto: secure storage, pythemis (Python) + webthemis (C++ + HTML/JS): [github repo](https://github.com/cossacklabs/sesto) [blog post](https://cossacklabs.com/presenting-sesto.html)

# Themis Server

If you'd like to experiment with Themis in more interactive environment, check out [Themis Server](https://themis.cossacklabs.com), interactive debugging environment for Themis. 

# Documentation

[Project's github wiki](https://www.github.com/cossacklabs/themis/wiki) contains ever-evolving official documentation, which contains everything from how to use it to ways to contribute to it, with a brief explanation of cryptosystems and architecture behind main Themis library in between. 
