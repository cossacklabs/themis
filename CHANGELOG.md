# Themis ChangeLog


## [0.9.6](https://github.com/cossacklabs/themis/releases/tag/0.9.6), December 14th 2017

**TL;DR:** OpenSSL 1.1 support.

_Docs:_

- Significant update of wiki [Contributing section](https://github.com/cossacklabs/themis/wiki/Contribute).

_Infrastructure:_

- Drop _Ubuntu Precise_ support.
- Fix `.rpm` package versioning ([#240](https://github.com/cossacklabs/themis/pull/240)).
- Add handy command to prepare and run all tests `make test` ([#243](https://github.com/cossacklabs/themis/issues/243)).
- Small changes and updates in Makefile to make it even better, and fix installing dependencies ([#236](https://github.com/cossacklabs/themis/pull/236), [#239](https://github.com/cossacklabs/themis/pull/239), [#250](https://github.com/cossacklabs/themis/pull/250)).


_Code:_

- **Core:** 
  - add OpenSSL 1.1 support ([#208](https://github.com/cossacklabs/themis/issues/208)).
- **Android wrapper**: 
  - fix Secure Cell in token protect mode ([#251](https://github.com/cossacklabs/themis/pull/251));
  - fix casting warnings in JNI code ([#246](https://github.com/cossacklabs/themis/pull/246)).
- **iOS wrapper:** 
  - update wrapper to be compatible with Swift4 ([#230](https://github.com/cossacklabs/themis/issues/230));
  - add nullability support ([#255](https://github.com/cossacklabs/themis/pull/255));
  - make NSError autoreleasing ([#257](https://github.com/cossacklabs/themis/pull/257), [#259](https://github.com/cossacklabs/themis/pull/259)) from [@valeriyvan](https://github.com/valeriyvan);
  - fix warnings due to renaming `error.h` files ([#247](https://github.com/cossacklabs/themis/pull/247));
  - update and refactor tests ([#231](https://github.com/cossacklabs/themis/pull/231), [#232](https://github.com/cossacklabs/themis/pull/232)).
- **GoThemis:** 
   - add compatibility with old Go (1.2) ([#253](https://github.com/cossacklabs/themis/issues/253)); 
   - fix tests ([#261](https://github.com/cossacklabs/themis/pull/261)).
- **JSThemis:** 
  - fix installation path on macOS ([#237](https://github.com/cossacklabs/themis/issues/237), [#238](https://github.com/cossacklabs/themis/pull/238/)).
- **PyThemis:** 
  - fix compatibility with 0.9.5 version ([#241](https://github.com/cossacklabs/themis/pull/241)), pushed as separate package [0.9.5.1](https://pypi.python.org/pypi/pythemis/0.9.5.1).


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
