# Themis ChangeLog

## [0.10.0](https://github.com/cossacklabs/themis/releases/tag/0.10.0), February 6th 2018

**TL;DR:** Multi-platform, multi-language compatibility improved.

⚠️ _Incompatibility issues:_ 

- If you are using Themis on x64 systems, consider upgrading every Themis library/wrapper to `0.10.0` at the moment. Incompatibility issues may arise between previous Themis versions and `0.10.0` on x64 machines ([#279](https://github.com/cossacklabs/themis/pull/279)).

- Rubythemis has breaking changes for Secure Cell Token Protect mode ([#281](https://github.com/cossacklabs/themis/pull/281)). We added checks for other language wrappers to make sure it won't happen again ([#282](https://github.com/cossacklabs/themis/pull/282), [#283](https://github.com/cossacklabs/themis/pull/283)).

Check [Migration Guide](https://github.com/cossacklabs/themis/wiki/Migration-guide) for more details.

_Docs:_

- Updated [Crypto systems](https://github.com/cossacklabs/themis/wiki/Cryptosystems) description, provide more usage examples and code samples.
- Refreshed code samples in language tutorials, and make them more clear.<br/>[Obj-C](https://github.com/cossacklabs/themis/wiki/Objective-C-Howto) | [Swift](https://github.com/cossacklabs/themis/wiki/Swift-Howto) | [Java and Android](https://github.com/cossacklabs/themis/wiki/Java-and-Android-Howto) | [Python](https://github.com/cossacklabs/themis/wiki/Python-Howto) | [PHP](https://github.com/cossacklabs/themis/wiki/PHP-Howto) | [Ruby](https://github.com/cossacklabs/themis/wiki/Ruby-Howto) | [C++](https://github.com/cossacklabs/themis/wiki/CPP-Howto) | [Go](https://github.com/cossacklabs/themis/wiki/Go-HowTo) | [Node js](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto).
- Added human-friendly description of [Secure Comparator](https://github.com/cossacklabs/themis/wiki/Secure-Comparator-cryptosystem) and provide usage examples for all languages, except PHP.

_Infrastructure:_

- Added support of _Ubuntu 17.10_.
- Removed support of _Ubuntu 16.10_ and _Ubuntu 17.04_ (no compiled binaries for these OS anymore).
- Added CLI utils for easy testing of Secure Cell, Secure Message and Key generation on local machine. Available for Python, Ruby, Go, NodeJS and PHP. Check [Console Utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) guide for details and usage description.
- Added [Integration test suit](https://github.com/cossacklabs/themis/tree/master/tests/_integration) for most language wrappers, which is the sponsor of many fixes in this release. 
- Added CircleCI 2.0 support with multiple workflows, which allows to test each language wrapper and integrations between them easy and quickly ([#295](https://github.com/cossacklabs/themis/pull/295), [#299](https://github.com/cossacklabs/themis/pull/299)).


_Code:_

- **Core:** 
  - fixed incompatibility issue for Secure Cell between x32 and x64 machines ([#279](https://github.com/cossacklabs/themis/pull/279));
  - added C examples of using buffer API in Secure Session ([#271](https://github.com/cossacklabs/themis/pull/271));
  - fixed possible SigFault in Secure Message when providing incorrect private key path ([#286](https://github.com/cossacklabs/themis/pull/286)).
- **C++ wrapper:**
  - added set of unit tests ([#292](https://github.com/cossacklabs/themis/pull/292)).  
- **Android wrapper**: 
  - simplified Android build by providing ready-to-use Docker container. You don't need to compile BoringSSL manually anymore! ([#273](https://github.com/cossacklabs/themis/pull/273) by [@secumod](https://github.com/secumod)).
- **iOS wrapper:** 
  - updated Secure Comparator definitions and provided code samples ([#287](https://github.com/cossacklabs/themis/pull/287), [#288](https://github.com/cossacklabs/themis/pull/288)).
- **GoThemis:** 
  - added `get_remote_id` function making SecureSession easier to use ([#272](https://github.com/cossacklabs/themis/pull/272)).
  - added [CLI utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) and integration tests that allows you to test Secure Cell, Secure Message and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **JSThemis:** 
  - added separate installation step in Makefile, now you can install jsthemis via `make jsthemis_install` ([#302](https://github.com/cossacklabs/themis/pull/302));
  - added [CLI utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) and integration tests that allows you to test Secure Cell, Secure Message and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **RubyThemis:**
  - fixed arguments order for Secure Cell in Token Protect mode, this leads to incompatibility with older versions of rubythemis ([#281](https://github.com/cossacklabs/themis/pull/281)). Check [migration guide](https://github.com/cossacklabs/themis/wiki/Migration-guide) for details;
  - added [CLI utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) and integration tests that allows you to test Secure Cell, Secure Message and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306))
- **PyThemis:** 
  - improved installation via Makefile for python3 users ([#300](https://github.com/cossacklabs/themis/pull/300));
  - added [CLI utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) and integration tests that allows you to test Secure Cell, Secure Message and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **PHPThemis:**
  - added PHP 7.0, 7.1, 7.2 support ([#278](https://github.com/cossacklabs/themis/pull/278), [#280](https://github.com/cossacklabs/themis/pull/280));
  - added package for `phpthemis`, now you don't need to compile it from sources. Check [PHP guide](https://github.com/cossacklabs/themis/wiki/PHP-Howto#installing-from-packages) for details;
  - improved [unit tests](https://github.com/cossacklabs/themis/wiki/PHP-Howto#installing-phpthemis): now it's easy to run tests, because all dependencies are handled by php-composer ([#284](https://github.com/cossacklabs/themis/pull/284), [#285](https://github.com/cossacklabs/themis/pull/285), [#303](https://github.com/cossacklabs/themis/pull/303));
  - added memory test suit, which allows us to pay attention on PHPThemis memory usage ([#298](https://github.com/cossacklabs/themis/pull/298));
  - added [CLI utils](https://github.com/cossacklabs/themis/wiki/Encrypt-Decrypt-Console-Utils) and integration tests that allows you to test Secure Cell, Secure Message and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).


## [0.9.6](https://github.com/cossacklabs/themis/releases/tag/0.9.6), December 14th 2017

**TL;DR:** OpenSSL 1.1 support.

_Docs:_

- Significant update of the [Contributing section](https://github.com/cossacklabs/themis/wiki/Contribute) in Wiki.

_Infrastructure:_

- Removed support for _Ubuntu Precise_.
- Fixed `.rpm` package versioning ([#240](https://github.com/cossacklabs/themis/pull/240)).
- Added a handy command for preparing and running of all the tests `make test` ([#243](https://github.com/cossacklabs/themis/issues/243)).
- Added small changes and updates into [Makefile](https://github.com/cossacklabs/themis/blob/master/Makefile) to make it even better and fixed the installing dependencies ([#236](https://github.com/cossacklabs/themis/pull/236), [#239](https://github.com/cossacklabs/themis/pull/239), [#250](https://github.com/cossacklabs/themis/pull/250)).


_Code:_

- **Core:** 
  - added OpenSSL 1.1 support ([#208](https://github.com/cossacklabs/themis/issues/208)).
- **Android wrapper**: 
  - fixed Secure Cell in token protect mode ([#251](https://github.com/cossacklabs/themis/pull/251));
  - fixed casting warnings in JNI code ([#246](https://github.com/cossacklabs/themis/pull/246)).
- **iOS wrapper:** 
  - updated wrapper to be compatible with Swift4 ([#230](https://github.com/cossacklabs/themis/issues/230));
  - added nullability support ([#255](https://github.com/cossacklabs/themis/pull/255));
  - made the NSError autoreleasing ([#257](https://github.com/cossacklabs/themis/pull/257), [#259](https://github.com/cossacklabs/themis/pull/259)) from [@valeriyvan](https://github.com/valeriyvan);
  - fixed warnings that appeared due to renaming `error.h` files ([#247](https://github.com/cossacklabs/themis/pull/247));
  - updated and refactored tests ([#231](https://github.com/cossacklabs/themis/pull/231), [#232](https://github.com/cossacklabs/themis/pull/232)).
- **GoThemis:** 
   - added compatibility with old Go (1.2) ([#253](https://github.com/cossacklabs/themis/issues/253)); 
   - fixed tests ([#261](https://github.com/cossacklabs/themis/pull/261)).
- **JSThemis:** 
  - fixed installation path for macOS ([#237](https://github.com/cossacklabs/themis/issues/237), [#238](https://github.com/cossacklabs/themis/pull/238/)).
- **PyThemis:** 
  - fixed compatibility with version 0.9.5 ([#241](https://github.com/cossacklabs/themis/pull/241)), pushed as a separate package [0.9.5.1](https://pypi.python.org/pypi/pythemis/0.9.5.1).


## [0.9.5](https://github.com/cossacklabs/themis/releases/tag/0.9.5), September 13th 2017

**TL;DR:** Mostly usability fixes for wrappers.

_Infrastructure:_

- You can now download pre-built Themis packages from **our package server**.
- Enhanced building process for **MacOS** (working now!) ([#215](https://github.com/cossacklabs/themis/issues/215)).
- Enhanced building process for **Debian 9.x** (working even better now!).
- Updated documentation and examples to make it easier to understand.
- Now we use Bitrise as a separate CI for iOS wrapper.
- Test and code coverage are automagically measured now!

_Code:_

- **Core:** disabled SHA1 support.
- **Secure Comparator:** magically improved code readability ([#196](https://github.com/cossacklabs/themis/issues/196), [#195](https://github.com/cossacklabs/themis/issues/195)).
- **iOS wrapper:** added support of dynamic frameworks and bitcode ([#222](https://github.com/cossacklabs/themis/issues/222), [#219](https://github.com/cossacklabs/themis/issues/219), [#205](https://github.com/cossacklabs/themis/issues/205)).
- **GoThemis:** refactored custom error (`themisError`) type.
- **PHP wrapper:** updated tests.
- **PyThemis:** considerably improved example projects.


## [0.9.4](https://github.com/cossacklabs/themis/releases/tag/0.9.4), November 22nd 2016

This is tiny intermediary release to lock ongoing changes in stable form for all languages:

* **BoringSSL** support on **Android** and **Linux**
* Fixed some leaks and code styling problems (thanks to [@bryongloden](https://github.com/bryongloden))
* Memory management updates for stability in languages, which rely on sloppy GC
* Fix Themis build errors under certain conditions
* **Secure Comparator** examples for many languages
* **Swift3** support + numerous enhancements from [@valeriyvan](https://github.com/valeriyvan), thanks a lot! 
* **GoThemis**: fixed rare behavior in Secure Session wrapper 
* GoThemis examples
* **JsThemis** syntax corrections and style fixes
* JsThemis Nan usage to enhance compatibility
* More and better **Themis Server examples**
* Enhanced **error messages** (now with proper spelling!)
* Corrections for **[RD_Themis](https://github.com/cossacklabs/rd_themis)**

## [0.9.3.1](https://github.com/cossacklabs/themis/releases/tag/0.9.3.1), August 24th 2016

Updating podspec to be compatible with CocoaPods 1.0


## [0.9.3](https://github.com/cossacklabs/themis/releases/tag/0.9.3), May 24th 2016

_Infrastructure_: 
* Lots of new high-level language wrappers
* Enhanced **[documentation](https://github.com/cossacklabs/themis/wiki)**
* Lots of various demo projects
* Updated **[Themis Server](https://themis.cossacklabs.com)**
* Better **make** system verbosity (now you can actually see what succeeded and what didn't)
* Infrastructure to **build Java** on all platforms

_Code_:
* **iOS wrapper** now has umbrella header.
* We added **Swift** language [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) and [howto](https://github.com/cossacklabs/themis/wiki/Swift-Howto).
* Themis wrapper for **Go** language: [howto](https://github.com/cossacklabs/themis/wiki/Go-HowTo) (examples coming soon).
* Themis wrapper for **NodeJS**: [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/nodejs) and [howto](https://github.com/cossacklabs/themis/wiki/NodeJS-Howto).
* Google Chrome-friendly spin-off called [WebThemis](https://github.com/cossacklabs/webthemis) was released. 
* Themis wrapper for **C++**: [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) and [howto](https://github.com/cossacklabs/themis/wiki/CPP-Howto).
* **[Secure Comparator](https://www.cossacklabs.com/files/secure-comparator-paper-rev12.pdf)** got [serious updates](https://cossacklabs.com/fixing-secure-comparator.html) to eliminate possible security drawbacks pointed out by cryptographic community.


## [0.9.2](https://github.com/cossacklabs/themis/releases/tag/0.9.2), November 4th 2015

_Infrastructure_: 

- **Much better documentation**
- We've introduced **Themis Server**, interactive environment to debug your apps and learn how Themis works.
- **Tests** for all platforms and languages.
- Themis is now integrated with **Circle CI**, with controls every repository change and tests it
- Added **conditional compilation for advanced features** (see 4.5 and our blog for more information) 
- **Doxygen**-friendly comments in code

_Code_:

- **Python Themis wrapper** is now Python 3 / PEP friendly.
- **Android Themis wrapper** for Secure Message now works in Sign/Verify mode, too.
- **PHP Themis** wrapper now supports Secure Session (although with some advice on use cases, see docs).
- **iOS wrapper** supports iOS 9, lots of minor fixes.
- **Better exceptions** and verbose errors in some wrappers.
- **Longer RSA keys** support
- **Better abstractions for cryptosystem parameters** like key lengths. 
- **Zero Knowledge Proof-based authentication** called Secure Comparator. Advanced experimental feature.
