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
  <a href="https://github.com/cossacklabs/themis/tree/master/tests"><img src="https://www.bitrise.io/app/b32b4ea8bffedad7/status.svg?token=sFMwuzxY2zy26LsqMkjuqg&branch=master" alt="Bitrise"></a>
  <a href="https://github.com/cossacklabs/themis/releases/latest"><img src="https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20macOS%20%7C%20Linux%20%7C%20Windows%20%7C%20Java-green.svg" alt="Platforms"></a>
  <a href='https://coveralls.io/github/cossacklabs/themis'><img src='https://coveralls.io/repos/github/cossacklabs/themis/badge.svg?branch=master' alt='Coverage Status' /></a>
  <a href='https://goreportcard.com/report/github.com/cossacklabs/themis'><img class="badge" tag="github.com/cossacklabs/themis" src="https://goreportcard.com/badge/github.com/cossacklabs/themis"></a>
</p>
<br>

#### Crypto library for storage and messaging for Swift, Obj-C, Android/Java, С++, JS, Python, Ruby, PHP, Go.
<br>

Themis is an open-source high-level cryptographic services library for mobile and server platforms, which provides secure data exchange and storage. The current stable release is [0.9.6](https://github.com/cossacklabs/themis/releases/tag/0.9.6), dated 14th of December 2017. 

Themis provides four important cryptographic services:

* **[Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem)**: a simple encrypted messaging solution for the widest scope of applications. Exchange the keys between the parties and you're good to go. Two pairs of the underlying crytosystems: ECC + ECDSA / RSA + PSS + PKCS#7. 
* **[Secure Session](https://github.com/cossacklabs/themis/wiki/Secure-Session-cryptosystem)**: session-oriented, forward secrecy datagram exchange solution with better security guarantees, but more demanding infrastructure. Secure Session can perfectly function as socket encryption, session security, or (with some additional infrastructure) as a high-level messaging primitive. ECDH key agreement, ECC & AES encryption.
* **[Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)**: a multi-mode cryptographic container suitable for storing anything from encrypted files to database records and format-preserved strings. Secure Cell is built around AES in GCM (Token and Seal modes) and CTR (Context imprint mode).
* **[Secure Comparator](https://github.com/cossacklabs/themis/wiki/Secure-Comparator-cryptosystem)**: a Zero-Knowledge based cryptographic protocol for authentication and comparing secrets.

**Want to go straight to the detailed documentation?** [Please proceed here](https://www.github.com/cossacklabs/themis/wiki). 

We created Themis to build other products on top of it - i.e. [Acra](https://github.com/cossacklabs/acra) and [Hermes](https://github.com/cossacklabs/hermes-core).

Themis works on most operating systems (see [Availability](https://github.com/cossacklabs/themis#availability)) and is available for [Swift (iOS, macOS)](https://github.com/cossacklabs/themis/wiki/Swift-Howto), [Objective-C (iOS, macOS)](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto), [Java+Android](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto),  [Ruby](https://github.com/cossacklabs/themis/wiki/Ruby-Howto),  [Python](https://github.com/cossacklabs/themis/wiki/Python-Howto), 
[PHP](https://github.com/cossacklabs/themis/wiki/PHP-Howto), 
[C++](https://github.com/cossacklabs/themis/wiki/CPP-Howto), 
[Javascript (NodeJS)](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto), [Go](https://github.com/cossacklabs/themis/wiki/Go-HowTo),
[Google Chrome](https://github.com/cossacklabs/webthemis). 

We're also porting Themis features to different environments where people might need them: [Redis module](https://www.github.com/cossacklabs/rd_themis), [PostgreSQL module](https://www.github.com/cossacklabs/pg_themis)

Themis was designed to provide complicated cryptosystems in an easy-to-use infrastructure, with modern rapid development in mind:

* **EASY**: Themis does not require users to obsess over parameters, cipher combination, and yet it provides high levels of security.
* **DO YOUR THING**: Themis allows developers to focus on doing the necessary: developing their applications. 
* **BEST PRACTICE**: Themis is based on the best modern practices in implementing complicated security systems.

Themis relies on [the best available](https://github.com/cossacklabs/themis/wiki/Cryptographic-donors) open-source implementations of cryptographic primitives (ciphers). 

Themis is open source, Apache 2 Licensed. 

# Quickstart

## Install Themis from Cossack Labs repository

### Debian / Ubuntu

**1. Import the public key used by Cossack Labs to sign packages:**
```console
wget -qO - https://pkgs.cossacklabs.com/gpg | sudo apt-key add -
```
> Note: If you wish to validate key fingerprint, it is: `29CF C579 AD90 8838 3E37 A8FA CE53 BCCA C8FF FACB`.

**2. You may need to install the apt-transport-https package before proceeding:**
```console
sudo apt-get install apt-transport-https
```
**3. Add Cossack Labs repository to your `sources.list`.**
You should add a line that specifies your OS name and the release name:
```console
deb https://pkgs.cossacklabs.com/stable/$OS $RELEASE main
```
* `$OS` should be `debian` or `ubuntu`.
* `$RELEASE` should be one of Debian or Ubuntu release names. You can determine this by running `lsb_release -cs`, if you have `lsb_release` installed.

We currently build packages for the following OS and RELEASE combinations:

- *Debian "Wheezy" (Debian 7)*
- *Debian "Jessie" (Debian 8)*
- *Debian "Stretch" (Debian 9)*
- *Ubuntu Trusty Tahr (Ubuntu 14.04)*
- *Ubuntu Xenial Xerus (Ubuntu 16.04)*

For example, if you are running *Debian 9 "Stretch"*, run:

```console
echo "deb https://pkgs.cossacklabs.com/stable/debian stretch main" | \
  sudo tee /etc/apt/sources.list.d/cossacklabs.list
```
**4. Reload local package database:**
```console
sudo apt-get update
```
**5. Install the package**
```console
sudo apt-get install libthemis
```

### CentOS / RHEL / OEL

> Note: We only build RPM packages for x86_64.

**1. Import the public key used by Cossack Labs to sign packages:**

```console
sudo rpm --import https://pkgs.cossacklabs.com/gpg
```
> Note: If you wish to validate key fingerprint, it is: `29CF C579 AD90 8838 3E37 A8FA CE53 BCCA C8FF FACB`.

**2. Create a Yum repository file for Cossack Labs package repository:**

```console
wget -qO - https://pkgs.cossacklabs.com/stable/centos/cossacklabs.repo | \
  sudo tee /etc/yum.repos.d/cossacklabs.repo
```
**3. Install the package:**

```console
sudo yum install libthemis
```

That's all! Themis is ready to use. The easiest way is to follow one of the tutorials and examples provided below.

## Install Themis from GitHub

1. Fetch the repository: git clone https://github.com/cossacklabs/themis.git.
2. Make sure OpenSSL/LibreSSL + OpenSSL/LibreSSL Dev package (libssl-dev) are installed at typical paths: `/usr/lib`, `/usr/include` (`/usr/local/*` for macOS). 
3. Make sure the typical GCC/clang environment is installed.
4. Type 'make install' and you're done (in most of the cases).
5. Visit [our wiki](https://github.com/cossacklabs/themis/wiki) for the documentation specific to your language of choice and take a look at docs/examples for examples. 

It is a really good idea to [go and read the docs after installing Themis](https://github.com/cossacklabs/themis/wiki/Building-and-installing), but we realise that Fortune favours the brave. Remember that you're always welcome in the [documentation Wiki](https://github.com/cossacklabs/themis/wiki).

# Languages

Themis is available for the following languages/platforms: 

| Platform | Documentation | Examples | Version |
| :----- | :----- | :------ | :--- |
| 🔶 Swift (iOS, macOS) | [Swift Howto](https://github.com/cossacklabs/themis/wiki/Swift-Howto) | [docs/examples/swift](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) | [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| 📱 Objective-C (iOS, macOS) | [Objective-C Howto](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto) | [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc)| [![CocoaPods](https://img.shields.io/cocoapods/v/themis.svg)](https://cocoapods.org/pods/themis) |
| ☕️ Java / Android | [Java & Android Howto](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto) | [jni example](https://github.com/cossacklabs/themis/tree/master/jni) ||
| ♦️ Ruby | [Ruby Howto](https://github.com/cossacklabs/themis/wiki/Ruby-Howto) | [docs/examples/ruby](https://github.com/cossacklabs/themis/tree/master/docs/examples/ruby) | [![Gem](https://img.shields.io/gem/v/rubythemis.svg)](https://rubygems.org/gems/rubythemis) |
| 🐍 Python | [Python Howto](https://github.com/cossacklabs/themis/wiki/Python-Howto) | [docs/examples/python](https://github.com/cossacklabs/themis/tree/master/docs/examples/python) | [![PyPI](https://img.shields.io/pypi/v/pythemis.svg)](https://pypi.python.org/pypi?%3Aaction=search&term=pythemis&submit=search) |
| 🐘 PHP | [PHP Howto](https://github.com/cossacklabs/themis/wiki/PHP-Howto) | [docs/examples/php](https://github.com/cossacklabs/themis/tree/master/docs/examples/php) | |
| ➕ C++ | [CPP Howto](https://github.com/cossacklabs/themis/wiki/CPP-Howto) | [docs/examples/c++](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) ||
| 🍭 Javascript (NodeJS) | [NodeJS Howto](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto) | [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js) | [![npm](https://img.shields.io/npm/v/jsthemis.svg)](https://www.npmjs.com/package/jsthemis) |
| 🐹 Go | [Go-Howto](https://github.com/cossacklabs/themis/wiki/Go-Howto)| [docs/examples/go](https://github.com/cossacklabs/themis/tree/master/docs/examples/go) ||
| 🕸 С++ PNaCl for Google Chrome||[WebThemis project](https://github.com/cossacklabs/webthemis)|


# Plugins

Themis-based plugins are built to enable Themis' features across various platforms and products: 

- [RD_Themis](https://github.com/cossacklabs/rd_themis) for Redis database
- [PG_Themis](https://github.com/cossacklabs/pg_themis) for PostgreSQL database


# Availability

Themis supports the following architectures: x86/x64, armv*, various Android architectures.

It is checked to compile on the latest stable versions of:

* Debian 7.8+, CentOS 7, Ubuntu 14, Arch Linux 2014+
* Windows XP+
* macOS 10.9+
* Android 4-8+ / CyanogenMod 11+
* iOS8—iOS11+, x32/x64

We plan to expand this list with broader set of platforms. If you'd like to help improve or bring Themis to your favourite platform / language — [get in touch](info@cossacklabs.com).

# Tutorials

As long as it remains feasible, we'll be accumulating the list of all our tutorials on how to use Themis in different cases here:

* [Releasing Themis into public: usability testing](https://www.cossacklabs.com/02-usability-testing.html), which goes a bit into how to use Secure Message for iOS and Python. Go directly into [corresponding github repository](https://github.com/cossacklabs/themis-ux-testing) to play with code. 
* [Building encrypted chat service with Themis and mobile websocket example](https://www.cossacklabs.com/building-secure-chat), which outlines stages necessary to build encrypted chat service around Ruby websocket server with clients in iOS and Android. [Github repository](https://github.com/cossacklabs/mobile-websocket-example) with accompanying code.

# Sample projects

During the development stage we frequently do Proof-of-Concept projects to test different assumptions. They serve as interesting demos of what Themis is capable of:

| Demo | Description | Repo | Blog post |
| :----- | :----- | :------ | :--- |
| 0fc | Anonymous web chat<br/>* pythemis (Python)<br/>* webthemis (C++ + HTML/JS)| [repo](https://github.com/cossacklabs/0fc) | [blog post](https://cossacklabs.com/building-endtoend-webchat.html) |
| sesto | Secure storage<br/>* pythemis (Python)<br/>* webthemis (C++ + HTML/JS) | [repo](https://github.com/cossacklabs/sesto) |[blog post](https://cossacklabs.com/presenting-sesto.html) 
| swift alps demo | Secure communication (iOS app with Python server based on Secure Session)<br/>* Swift wrapper<br/>* pythemis (Python) | [repo](https://github.com/cossacklabs/theswiftalpsdemo) |[slides](https://speakerdeck.com/vixentael/the-swift-alps-security-workshop) |

# Themis Server

If you'd like to experiment with Themis in a more interactive environment, check out [Themis Server](https://themis.cossacklabs.com), interactive debugging environment for Themis. 

# Contributing to us

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/themis/issues). Head over to our [Contribution guidelines](https://github.com/cossacklabs/themis/wiki/Contribute) as your starting point.


# Documentation

[Project's GitHub Wiki](https://www.github.com/cossacklabs/themis/wiki) contains the ever-evolving official documentation, which contains everything from deployment guidelines to use-cases, with a brief explanation of cryptosystems and architecture behind the main Themis library sandwiched in. 

# Contacts

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/themis/issues) or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).
   
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/) [![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

