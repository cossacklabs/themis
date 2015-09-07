# Themis 0.9.1

[![Join the chat at https://gitter.im/cossacklabs/themis](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/cossacklabs/themis?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![aaa](https://circleci.com/gh/:owner/:repo/tree/:branch.svg?style=shield)]

Themis is a high-level cryptographic services library: a library providing easy to use, highly abstracted set of functions to solve real-world security problems. We would like you to focus on building your software with security 
taken care of by cryptographers, instead of scrupulously assembling building blocks into cryptosystems yourself, resolving implementations, platform availability, vulnerabilities and performance constraints yourself.

Themis was designed to provide complicated cryptosystems in easy-to-use infrastructure, suitable for modern rapid development. It is open source, Apache 2 Licensed.

Themis is set to provide a wide set of instruments, three main of which are being present in current, 0.9 release:

* Secure Message: a simple encrypted messaging solutions for widest scope of application
* Secure Session: safer, session-oriented messaging solution with better security guarantees (and some more strict demands in it's implementation)
* Secure Cell: a multi-mode encrypted container, suitable for storing anything from encrypted files to database records and format-preserved strings.

# Quickstart Crash Course

1. Fetch the repository: git clone https://github.com/cossacklabs/themis.git
2. Have OpenSSL/LibreSSL + OpenSSL/LibreSSL Dev pack (libssl-dev) installed at typical paths: `/usr/lib`, `/usr/include`. 
3. Have typical gcc environment installed
4. Fire 'make install' and you're good most of the cases
5. Dive into [our wiki](https://github.com/cossacklabs/themis/wiki) for the docs of language of your choice, take a look at docs/examples for examples. 

It is really advisable to [go the long way and read the docs](https://github.com/cossacklabs/themis/wiki/2.1-Building-and-installing), but god blesses the brave.

# High level language reference

Themis currently comes for 5 languages: 

- [Python](https://github.com/cossacklabs/themis/wiki/2.4.1-Python-Howto): fully documented, examples for typical Python frameworks (twisted, tornado), thoroughly tested by practicing Python developers, Python interface is considered stable. 
- [Ruby](https://github.com/cossacklabs/themis/wiki/2.4.3-Ruby-Howto): fully documented, examples for typical Ruby development models (event-based async, around Rails and Eventmachine), being tested right now, yet is considered stable enough to run everything.
- [PHP](https://github.com/cossacklabs/themis/wiki/2.4.2-PHP-Howto): developed and tested as both stand-alone PHP (invoked from command line) and typical Apache request-bound forking. Examples are provided for the latter as more common practice. Should work in raw stand-alone way too.
- [Objective-C](https://github.com/cossacklabs/themis/wiki/2.4.4-Objective-C-Howto): 2 objects out of 3 are documented, one is tested in real-world case (Demo project w/ [blog post](http://cossacklabs.com/blog.html) coming soon).
- [Java / Android](https://github.com/cossacklabs/themis/wiki/2.4.5-Java---Android): 3 objects out of 3, all are tested and covered in simple examples. Demo project and real-world "let's hackathon this" testing [blog post](http://cossacklabs.com/blog.html) coming soon.

# Availability

Themis supports the following architectures: x86/x64, armv*, various androids

It is checked to compile on:

* Debian 7.8, CentOS 6.6, CentOS Linux 7.1.1503, Ubuntu 14.04 LTS 
* MS Windows 7 - 64bit
* OSX 10.10
* Android 4.4.2
* Android 4.4.4 / CyanogenMod 11
* Android 5
* iOS 7+, x32/x64

Themis is available as original C library and has Python, Objective-C, Ruby, PHP and Android wrappers.

We plan to expand this minuscule availability scope with broader set of platforms. If you'd like to help Themis arrive (or get better) on your favourite platform / language — get in touch.

# Docs

[https://www.github.com/cossacklabs/themis/wiki] contains ever-evolving official documentation, which contains everything from how to use it to ways to contribute to it, with a brief explanation of cryptosystems and architecture behind main Themis library in between. 
