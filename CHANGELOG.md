# Themis Change Log


## [0.9.3], May. 24th 2016

_Infrastructure_: 

* **Swift** [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) and **Swift** [HowTo](https://github.com/cossacklabs/themis/wiki/Swift-Howto) added

_Code_:
* **iOS wrapper** now has umbrella header


## [0.9.2], Nov. 4th 2015

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
