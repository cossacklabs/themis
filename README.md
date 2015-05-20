# Themis 0.9 

Themis is a high-level cryptographic services library. Themis was designed to provide complicated cryptosystems in easy-to-use infrastructure, suitable for modern rapid development. It is open source, Apache 2 Licensed.

Themis is set to provide a wide set of instruments, three main of which are being present in current, 0.9 release:

* Secure Message: a simple encrypted messaging solutions for widest scope of application
* Secure Session: safer, session-oriented messaging solution with better security guarantees (and some more strict demands in it's implementation)
* Secure Cell: a multi-mode encrypted container, suitable for storing anything from encrypted files to database records and format-preserved strings.

# Quickstart Crash Course

1. Fetch the repository: git clone https://github.com/cossacklabs/themis.git
2. Have OpenSSL/LibreSSL + OpenSSL/LibreSSL Dev pack (libssl-dev) installed at typical paths: /usr/lib /usr/include. 
3. Have typical gcc environment installed
4. Fire 'make install' and you're good most of the cases
5. Dive into [our wiki](https://github.com/cossacklabs/themis/wiki) for the docs, take a look at docs/examples for examples. 

It is really advisable to [go the long way and read the docs](https://github.com/cossacklabs/themis/wiki/2.1-Building-and-installing), but god blessed the brave.

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

**Linux**:

* Debian 7.8 (3.2.0-4-amd64 #1 SMP Debian 3.2.65-1+deb7u2 x86_64 GNU/Linux)
* CentOS release 6.6 (2.6.32-504.el6.i686 #1 SMP Wed Oct 15 03:02:07 UTC 2014 i686 i686 i386 GNU/Linux)
* CentOS Linux release 7.1.1503 (3.10.0-229.el7.x86_64 #1 SMP Fri Mar 6 11:36:42 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux)
* Ubuntu 14.04 LTS (3.16.0-34-generic #47~14.04.1-Ubuntu SMP Fri Apr 10 17:49:16 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux)

**Windows**: MS Windows 7 - 64bit

**MacOS X** 10.10 Darwin Kernel Version 14.3.0: (root:xnu-2782.20.48~5/RELEASE_X86_64 x86_64)

**Android**:

* Samsung Galaxy Note 2 (Korean, SHV-E250K), Android 4.4.2
* Samsung Galaxy Gio (GT-S5660, Cyanogenmod 11), Android 4.4.4)

**iOS 7.0+** on armv7 armv7s arm64

Themis is available as original C library and has Python, Objective-C, Ruby, PHP and Android wrappers.

We plan to expand this minuscule availability scope with broader set of platforms. If you'd like to help Themis arrive (or get better) on your favourite platform / language — get in touch.

