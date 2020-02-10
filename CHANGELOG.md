# Themis ChangeLog

## Unreleased

Changes that are currently in development and have not been released yet.

**TL;DR:**
- Added API for generating symmetric keys for use with Secure Cell
- Added API for Secure Cell encryption with human-readable passphrases

_Code:_

- **Core**

  - **Soter** (low-level security core used by Themis)

    - New function `soter_pbkdf2_sha256()` can be used to derive encryption keys from passphrases with PBKDF2 algorithm ([#574](https://github.com/cossacklabs/themis/pull/574)).

  - **Key generation**

    - New function `themis_gen_sym_key()` can be used to securely generate symmetric keys for Secure Cell ([#560](https://github.com/cossacklabs/themis/pull/560)).

  - **Secure Cell**

    - New functions:

      - `themis_secure_cell_encrypt_seal_with_passphrase()`
      - `themis_secure_cell_decrypt_seal_with_passphrase()`

      provide Seal mode API that is safe to use with passphrases ([#577](https://github.com/cossacklabs/themis/pull/577)).

- **C++**

  - New function `themispp::gen_sym_key()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Updated test suite to test C++14 and C++17 (in addition to C++11 and C++03) ([#572](https://github.com/cossacklabs/themis/pull/572)).

- **Go**

  - New function `keys.NewSymmetricKey()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **iOS and macOS**

  - New function `TSGenerateSymmetricKey()` (available in Objective-C and Swift) can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **Java**

  - JDK location is now detected automatically in most cases, you should not need to set JAVA_HOME or JDK_INCLUDE_PATH manually ([#551](https://github.com/cossacklabs/themis/pull/551)).
  - JNI libraries are now available as `libthemis-jni` packages for supported Linux systems ([#552](https://github.com/cossacklabs/themis/pull/552), [#553](https://github.com/cossacklabs/themis/pull/553)).
  - Fixed a NullPointerException bug in `SecureSocket` initialisation ([#557](https://github.com/cossacklabs/themis/pull/557)).
  - Some Themis exceptions have been converted from checked `Exception` to _unchecked_ `RuntimeException`, relaxing requirements for `throws` specifiers ([#563](https://github.com/cossacklabs/themis/pull/563)).
  - Introduced `IKey` interface with accessors to raw key data ([#564](https://github.com/cossacklabs/themis/pull/564)).
  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#565](https://github.com/cossacklabs/themis/pull/565)).

- **Node.js**

  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#562](https://github.com/cossacklabs/themis/pull/562)).

- **PHP**

  - New function `phpthemis_gen_sym_key()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **Python**

  - Fixed compatibility issues on 32-bit platforms ([#555](https://github.com/cossacklabs/themis/pull/555)).
  - New function `skeygen.GenerateSymmetricKey()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **Ruby**

  - New function `Themis::gen_sym_key()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **Rust**

  - New object `themis::keys::SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

- **WebAssembly**

  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).

_Infrastructure:_

- Automated benchmarking harness is now tracking Themis performance. See [`benches`](https://github.com/cossacklabs/themis/tree/master/benches/) ([#580](https://github.com/cossacklabs/themis/pull/580)).

## [0.12.0](https://github.com/cossacklabs/themis/releases/tag/0.12.0), September 27th 2019

**TL;DR:**
- Added WasmThemis to support WebAssembly (works with Electron and Node.js);
- added experimental support for Windows (using MSYS2 compiler and NSIS installer);
- added support of Go Modules;
- added package for ThemisPP;
- added support for Node.js v12 LTS (in addition to v10 and v8) in jsThemis;
- added extra safety checks and fixes for tricky bugs.

**Breaking changes:**

- **Linux:** when building from sources, the default installation path of Themis Core library has been changed from `/usr` to `/usr/local`. If you’re affected, read the instructions on how to make a clean upgrade below.
- **Go:** some of GoThemis APIs have been renamed to comply with Go naming convention (old API are marked as deprecated and will be removed in the next release). If you’re using Go – please switch to new functions.
- **Ruby:** deprecated `rubythemis` gem has been completely removed in favour of `rbthemis`.

_Code:_

- **Core**

  - **Soter** (low-level security core used by Themis)

    Improved security and code quality, implemented better handling of secrets and memory management. These changes decrease the chance of potential memory leaks.

    - Introduced new internal function: `soter_wipe()`.
      It can be used to securely wipe sensitive data from memory after it's no longer needed.
      All Themis cryptosystems now use this new API ([#488](https://github.com/cossacklabs/themis/pull/488)).

    - Improved usage and error handling of OpenSSL/BoringSSL PRNGs.
      Thank you, [**@veorq**](https://github.com/veorq) for bringing up this issue ([#485](https://github.com/cossacklabs/themis/pull/485)).

    - Improved memory safety and fixed potential corner-case issues in OpenSSL/BoringSSL usage.
      Thanks, [**@outspace**](https://github.com/outspace) for identifying these issues ([#501](https://github.com/cossacklabs/themis/pull/501), [#524](https://github.com/cossacklabs/themis/pull/524), [#525](https://github.com/cossacklabs/themis/pull/525), [#535](https://github.com/cossacklabs/themis/pull/535)).

  - **Key generation**

    - Improved key validity checks across all Themis cryptosystems. Now it’s harder to use wrong keys or misuse them ([#486](https://github.com/cossacklabs/themis/pull/486)).

    - Improved error handling for EC key generator that could produce an invalid public key without returning an error.
      Thanks, [**@vixentael**](https://github.com/vixentael) for finding this issue ([#500](https://github.com/cossacklabs/themis/pull/500)).

  - **Secure Cell**

    - Improved handling of edge cases with large data buffers ([#496](https://github.com/cossacklabs/themis/pull/496), [#497](https://github.com/cossacklabs/themis/pull/497)).

    - Improved performance of Secure Cell's encryption/decryption ([#496](https://github.com/cossacklabs/themis/pull/496)).

  - **Library ABI**

    - Themis shared libraries now have a formal ABI version.
      This ensures that no compatibility issues arise if we ever need to introduce breaking changes in the ABI ([#454](https://github.com/cossacklabs/themis/pull/454)).

    - Removed private symbols from public export lists.
      Themis has been accidentally exporting various private utility functions.
      Private functions not intended for public use are now hidden ([#458](https://github.com/cossacklabs/themis/pull/458), [#472](https://github.com/cossacklabs/themis/pull/472)).

  - <a id="0.12.0-packaging-updates">**Installation & packaging**</a>

    - Themis now installs to `/usr/local` by default when building from source on Linux ([#448](https://github.com/cossacklabs/themis/pull/448)).

      This _may_ be a **breaking change** if your system has non-standard precedence rules.
      If you install Themis from source code directly, please do a clean upgrade the following way:

      ```bash
      make uninstall PREFIX=/usr
      make install   PREFIX=/usr/local
      ```

      Please consider using [binary repositories](https://docs.cossacklabs.com/pages/documentation-themis/#installing-themis-from-repositories) to install Themis.
      If your system or package manager is not supported yet, please let us know via dev@cossacklabs.com.

    - Themis packages now support multiarch installations ([#512](https://github.com/cossacklabs/themis/pull/512)).

      Multiarch enables parallel installation of 32-bit and 64-bit versions of the library.
      This is particularly important on CentOS where some tools like _pkg-config_ would fail to locate Themis due to non-standard installation path.

    - Updated Makefile to support a number of [standard GNU variables](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html) like `libdir` ([#453](https://github.com/cossacklabs/themis/pull/453), [#455](https://github.com/cossacklabs/themis/pull/455)).

    - Improved accuracy of package dependencies to make sure you don't have to install anything manually after installing Themis from package repositories ([#446](https://github.com/cossacklabs/themis/pull/446)).

    - NSIS installer is now available for Windows. To build NSIS installer, use `make nsis_installer` command in MSYS2 environment.
      You can read more about MSYS2 target [here](https://github.com/cossacklabs/themis/pull/469) ([#474](https://github.com/cossacklabs/themis/pull/474)).

  - **Dependency updates**

    - Embedded BoringSSL submodule has been updated to the latest upstream version ([#528](https://github.com/cossacklabs/themis/pull/528)).

    - Only the necessary parts of embedded BoringSSL are now built, leading to 2x build speedup ([#447](https://github.com/cossacklabs/themis/pull/447)).

  - **Other changes**

    - Miscellaneous improvements and cleanups in the Makefile ([#450](https://github.com/cossacklabs/themis/pull/450), [#451](https://github.com/cossacklabs/themis/pull/451), [#452](https://github.com/cossacklabs/themis/pull/452), [#459](https://github.com/cossacklabs/themis/pull/459), [#523](https://github.com/cossacklabs/themis/pull/523), [#527](https://github.com/cossacklabs/themis/pull/527)).

    - Core libraries are now linked dynamically to test binaries ([#460](https://github.com/cossacklabs/themis/pull/460)).

- **Android**

  - Embedded BoringSSL submodule has been updated to the latest upstream version ([#528](https://github.com/cossacklabs/themis/pull/528)).

  - Only the necessary parts of embedded BoringSSL are now built, leading to 2x build speedup ([#447](https://github.com/cossacklabs/themis/pull/447)).

- **C++**

  - ThemisPP is now available as a system package through [Cossack Labs repositories](https://docs.cossacklabs.com/pages/documentation-themis/#installing-themis-from-repositories) ([#506](https://github.com/cossacklabs/themis/pull/506)).

    Use
      - `libthemispp-dev` for Debian and Ubuntu,
      - `libthemispp-devel` for CentOS.

- **Go**

  - **Breaking changes**

    - Some APIs have been renamed to conform with the Go naming conventions ([#424](https://github.com/cossacklabs/themis/pull/424)).

      The old names are now deprecated and scheduled for removal in the next release.
      Please migrate to using the new names when you upgrade.

      | Old API                                | New API                    |
      | -------------------------------------- | -------------------------- |
      | `cell.CELL_MODE_SEAL`                  | `cell.ModeSeal`            |
      | `compare.COMPARE_MATCH`                | `compare.Match`            |
      | `keys.KEYTYPE_EC`                      | `keys.TypeEC`              |
      | `session.STATE_ESTABLISHED`            | `session.StateEstablished` |
      | `(*session.SecureSession) GetRemoteId` | `GetRemoteID`              |

  - GoThemis is now compatible with Go 1.11 modules starting with this release.
    For example, you are now able to pin a specific version of GoThemis in your projects ([#505](https://github.com/cossacklabs/themis/pull/505)).

- **iOS, macOS**

  - Example code and projects for Objective-C and Swift are now up-to-date, cleaned up, and modernised ([#463](https://github.com/cossacklabs/themis/pull/463), [#467](https://github.com/cossacklabs/themis/pull/467))

  - OpenSSL version is pinned to 1.0.2.17 for Carthage package and to 1.0.2.18 to CocoaPods package as a workaround for a compilation issue with the latest versions ([#539](https://github.com/cossacklabs/themis/pull/539), [#484](https://github.com/cossacklabs/themis/pull/484)).

- **Java**

  - JNI wrapper for desktop Java does not require a separate installation of Themis Core from now on ([#450](https://github.com/cossacklabs/themis/pull/450)).

- **Node.js**

  - JsThemis now supports latest Node.js v12 LTS (in addition to v10 and v8) ([#499](https://github.com/cossacklabs/themis/pull/499),
     [#502](https://github.com/cossacklabs/themis/pull/502)).

  - JsThemis can now be used on Windows provided that Themis Core is installed to `C:\Program Files\Themis` ([#475](https://github.com/cossacklabs/themis/pull/475)).

  - Improved error reporting when keys are misused with Secure Message objects ([#465](https://github.com/cossacklabs/themis/pull/465)).

- **PHP**

  - Updated PHP installer to use the latest Composer version ([#519](https://github.com/cossacklabs/themis/pull/519)).

- **Ruby**

  - **Breaking changes**

    - `rubythemis` gem has been completely removed after being deprecated in Themis 0.11.
      Please use `require 'rbthemis'` in your projects ([#508](https://github.com/cossacklabs/themis/pull/508)).

- **Rust**

  - Minor internal code style modernizations ([#466](https://github.com/cossacklabs/themis/pull/466)).

- **WebAssembly**

  - **WasmThemis** brings Themis to Web using [_WebAssembly_](https://webassembly.org).
    Thank you to [**@ilammy**](https://github.com/ilammy) for adding it.

    WasmThemis supports the full functionality of other Themis wrappers:
    Secure Cell, Secure Message, Secure Session, and Secure Comparator.
    WasmThemis package is [available via npm](https://www.npmjs.com/package/wasm-themis) as `wasm-themis`,
    sample code can be found in [docs/examples/js](https://github.com/cossacklabs/themis/tree/master/docs/examples/js),
    and the HowTo guide is available [on the documentation server](https://docs.cossacklabs.com/pages/js-wasm-howto/) ([#457](https://github.com/cossacklabs/themis/pull/457), [#461](https://github.com/cossacklabs/themis/pull/461), [#462](https://github.com/cossacklabs/themis/pull/462), [#473](https://github.com/cossacklabs/themis/pull/473), [#482](https://github.com/cossacklabs/themis/pull/482), [#489](https://github.com/cossacklabs/themis/pull/489), [#490](https://github.com/cossacklabs/themis/pull/490), [#491](https://github.com/cossacklabs/themis/pull/491), [#492](https://github.com/cossacklabs/themis/pull/492),[#494](https://github.com/cossacklabs/themis/pull/494), [#495](https://github.com/cossacklabs/themis/pull/495), [#498](https://github.com/cossacklabs/themis/pull/498), [#507](https://github.com/cossacklabs/themis/pull/507), [#513](https://github.com/cossacklabs/themis/pull/513)).

  - WasmThemis is tested with current Node.js LTS versions, popular Web browsers, and Electron framework.
    It is also tested for compatibility with other Themis wrappers ([#509](https://github.com/cossacklabs/themis/pull/509), [#510](https://github.com/cossacklabs/themis/pull/510), [#511](https://github.com/cossacklabs/themis/pull/511)).

- **Windows**

  - It is now possible to compile Themis Core for Windows using MSYS2 environment.
    See the instructions [here](https://github.com/cossacklabs/themis/issues/522#issuecomment-527519113) ([#469](https://github.com/cossacklabs/themis/pull/469)).

  - NSIS installer is now provided for the distribution of Themis on Windows.
    It's compatible with the new MSYS2 builds ([#474](https://github.com/cossacklabs/themis/pull/474)).

  - It is now possible to compile JsThemis on Windows, given that Themis Core is installed ([#475](https://github.com/cossacklabs/themis/pull/475)).

  - Miscellaneous compatibility fixes should make it possible to compile Themis Core with Microsoft Visual Studio.
    This platform does not have full official support yet, though ([#470](https://github.com/cossacklabs/themis/pull/470), [#471](https://github.com/cossacklabs/themis/pull/471)).

_Docs:_

- [Themis GitHub Wiki](https://github.com/cossacklabs/themis/wiki) is being deprecated.
  Please find the latest documentation for Themis on [Cossack Labs Documentation Server](https://docs.cossacklabs.com/products/themis/). If you're used to using the [Themis Wiki](https://github.com/cossacklabs/themis/wiki) or have bookmarked a few pages for further use, don't worry - its pages and table of contents stay where they were, but each will now link to its corresponding [Cossack Labs Documentation Server](https://docs.cossacklabs.com/products/themis/) counterpart.

- [Code of Conduct](https://github.com/cossacklabs/themis/blob/master/CODE_OF_CONDUCT.md) has been introduced to make sure that Themis project has a welcoming environment ([#518](https://github.com/cossacklabs/themis/pull/518)).

- Improved and updated installation guides for numerous languages and platforms.


_Infrastructure:_

- New Makefile targets and use cases:

  - `emmake make all` builds WasmThemis in Emscripten environment ([#475](https://github.com/cossacklabs/themis/pull/457));
  - `make all` can be run in MSYS2 environment on Windows now ([#469](https://github.com/cossacklabs/themis/pull/469));
  - `make nsis_installer` builds NSIS installer for Windows binaries ([#474](https://github.com/cossacklabs/themis/pull/474));
  - `make deb` and `make rpm` now build ThemisPP packages, too ([#506](https://github.com/cossacklabs/themis/pull/506));
  - `NO_NIST_STS` environment variable can be used to disable the long-running NIST statistical test suite when doing `make test` ([#456](https://github.com/cossacklabs/themis/pull/456)).

- Removed Makefile targets:

  - Deprecated `rubythemis` targets have been completely removed.
    Use `make rbthemis_install` to install RubyThemis instead ([#508](https://github.com/cossacklabs/themis/pull/508)).

- Multiple updates in the way Themis is installed and packaged.
  The most significant are switch to `/usr/local` for installation from source code and added support for multiarch installation packages ([Read more](#0.12.0-packaging-updates)).

- JsThemis and WasmThemis are now tested on all current LTS versions of Node.js ([#502](https://github.com/cossacklabs/themis/pull/502),
   [#510](https://github.com/cossacklabs/themis/pull/510)).

- Integration tests are getting stronger with WebAssembly platform being added to the suite ([#511](https://github.com/cossacklabs/themis/pull/511)).

- CI servers are now using the latest RVM for testing RubyThemis ([#503](https://github.com/cossacklabs/themis/pull/503), [#504](https://github.com/cossacklabs/themis/pull/504)).


## [0.11.1](https://github.com/cossacklabs/themis/releases/tag/0.11.1), April 1st 2019

**TL;DR:** Rust-Themis can now be installed entirely from packages (repositories and crates.io), without building anything from source.

_Code:_

- **Rust**

  - Improvements in lookup of core Themis library ([#444](https://github.com/cossacklabs/themis/pull/444)).

  - Minor changes in dependencies ([#443](https://github.com/cossacklabs/themis/pull/443)).

_Infrastructure:_

- Minor fixes in the packaging process ([#442](https://github.com/cossacklabs/themis/pull/442)).


## [0.11.0](https://github.com/cossacklabs/themis/releases/tag/0.11.0), March 28th 2019

**TL;DR:** Added Rust Themis, added Carthage distribution for iOS and Maven distribution for Android. Improved Secure Message API and propagated it to all our language wrappers.

We found that Themis is now [recommended by OWASP](https://github.com/OWASP/owasp-mstg/blob/1.1.0/Document/0x06e-Testing-Cryptography.md#third-party-libraries) as data encryption library for mobile platforms.

**Breaking changes:** We renamed `rubythemis` to `rbthemis` as a beginning of a tradition of gentle deprecations, with timely warning of all the users that can potentially be affected. We removed `themis_version()` function that allowed checking Themis' version at run-time (with no replacement function).

_Code:_

- **Core**

  - **Fixes in Soter (low-level security core used by Themis):**

    - Fixed possible null pointer deference caused by the misusage of short-circuit evaluation. Huge thanks to [@movie-travel-code](https://github.com/movie-travel-code) for pointing out ([#315](https://github.com/cossacklabs/themis/pull/315), [#314](https://github.com/cossacklabs/themis/pull/314)).

    - Fixed crash in Secure Message when RSA keys are used incorrectly (swapped or empty) – a shoutout for [@ilammy](https://github.com/ilammy) and [@secumod](https://github.com/secumod) for fixing this ([#334](https://github.com/cossacklabs/themis/pull/334)).

    - Fixed issue with RSA key generator silently truncating private keys – our gratitude going out to [@ilammy](https://github.com/ilammy) and [@secumod](https://github.com/secumod) again ([#335](https://github.com/cossacklabs/themis/pull/335)).

    - Fixed crash that occured on re-using Secure Comparator with BoringSSL – thanks to [@ilammy](https://github.com/ilammy) and [@secumod](https://github.com/secumod) for this fix ([#347](https://github.com/cossacklabs/themis/pull/347)).

    - Fixed overflow during Secure Cell decryption in Seal mode - thanks to [@ilammy](https://github.com/ilammy) and his skills in fuzz testing ([#367](https://github.com/cossacklabs/themis/pull/367)).

    - Improved the test suite to catch more corner cases, including with OpenSSL-specific issues ([#323](https://github.com/cossacklabs/themis/pull/323), [#319](https://github.com/cossacklabs/themis/pull/319)).

  - **Secure Session**

    - Added additional safety tests for Secure Session: return error if clientID is empty (thanks [@deszip](https://github.com/deszip) for asking tough questions and mis-using clientID) ([#386](https://github.com/cossacklabs/themis/pull/386)).

    - Described [thread safety code practices](https://docs.cossacklabs.com/pages/thread-safety-themis/) when using Secure Session.

  - **Secure Message**

    - Updated Secure Message API: divided the `wrap` function into `encrypt` and `sign`, and the `unwrap` function into `decrypt` and `verify`. The new API has more intuitive naming and should be harder to misuse, with encrypt/decrypt and sign/verify API clearly named and separated.

      A common mistake with the old API was that users could accidentally use sign/verify API instead of encryption because they didn't provide a private key. The new API features more strict checks and prevents this kind of mistake.

      This change doesn't affect the language wrappers you are using, so no code changes are required from you.

      Documentation for the new API calls is available [in the Wiki documentation](https://docs.cossacklabs.com/pages/secure-message-cryptosystem/#implementation-details) and for each language separately (in their HowTos) ([#389](https://github.com/cossacklabs/themis/pull/389)).

    - Fixed a potential memory leak in Secure Message encryption and decryption ([#398](https://github.com/cossacklabs/themis/pull/398)).

  - **Code quality**

    - Cleaned up circular dependencies in header files. This change has made the code cleaner and the compilation time faster ([#392](https://github.com/cossacklabs/themis/pull/392)).

    - Improved code quality by fixing warnings from various compiler flags (`-Wall -Wextra -Wformat-security -Wnull-dereference -Wshift-overflow` and so on) ([#377](https://github.com/cossacklabs/themis/pull/377)).

    - Formatted the code using `clang-format` and `clang-tidy`, added automated formatting for core and tests ([#418](https://github.com/cossacklabs/themis/pull/418), [#399](https://github.com/cossacklabs/themis/pull/399), [#397](https://github.com/cossacklabs/themis/pull/397), [#396](https://github.com/cossacklabs/themis/pull/396), [#395](https://github.com/cossacklabs/themis/pull/395)).

  - **Other changes**

    - Improved and refactored our Great Makefile to be more stable, more user-friendly, and to support OS-specific issues ([#417](https://github.com/cossacklabs/themis/pull/417), [#413](https://github.com/cossacklabs/themis/pull/413), [#348](https://github.com/cossacklabs/themis/pull/348), [#346](https://github.com/cossacklabs/themis/pull/346), [#345](https://github.com/cossacklabs/themis/pull/345), [#343](https://github.com/cossacklabs/themis/pull/343), [#321](https://github.com/cossacklabs/themis/pull/321)).

    - Removed `themis_version()` function and all related API for querying Themis and Soter versions at run-time. There is no replacement for it and this is obviously a breaking change ([#388](https://github.com/cossacklabs/themis/pull/388)).

- **Rust**

  - Introduced Rust Themis wrapper, all work done by brilliant [@ilammy](https://github.com/ilammy)!

    Rust Themis supports the same functionality as other Themis wrappers: Secure Cell, Secure Message, Secure Session, and Secure Comparator. Rust Themis package is available through [crates.io](https://crates.io/crates/themis), examples are stored in [docs/examples/rust](https://github.com/cossacklabs/themis/tree/master/docs/examples/rust), the HowTo guide is available [in Wiki](https://docs.cossacklabs.com/pages/rust-howto/) ([#419](https://github.com/cossacklabs/themis/pull/419), [#405](https://github.com/cossacklabs/themis/pull/405), [#403](https://github.com/cossacklabs/themis/pull/403), [#390](https://github.com/cossacklabs/themis/pull/390), [#383](https://github.com/cossacklabs/themis/pull/383), [#382](https://github.com/cossacklabs/themis/pull/382), [#381](https://github.com/cossacklabs/themis/pull/381), [#380](https://github.com/cossacklabs/themis/pull/380), [#376](https://github.com/cossacklabs/themis/pull/376), [#375](https://github.com/cossacklabs/themis/pull/375), [#374](https://github.com/cossacklabs/themis/pull/374), [#373](https://github.com/cossacklabs/themis/pull/373), [#372](https://github.com/cossacklabs/themis/pull/372), [#365](https://github.com/cossacklabs/themis/pull/365), [#363](https://github.com/cossacklabs/themis/pull/363), [#362](https://github.com/cossacklabs/themis/pull/362), [#358](https://github.com/cossacklabs/themis/pull/358), [#357](https://github.com/cossacklabs/themis/pull/357), [#356](https://github.com/cossacklabs/themis/pull/356), [#353](https://github.com/cossacklabs/themis/pull/353), [#349](https://github.com/cossacklabs/themis/pull/349), [#340](https://github.com/cossacklabs/themis/pull/340)).

- **iOS and macOS**

  - Added Carthage support. Now users can add Themis to their Cartfile using `github "cossacklabs/themis"`.

    More details available in [Objective-C HowTo](https://docs.cossacklabs.com/pages/objective-c-howto/) and [Swift HowTo](https://docs.cossacklabs.com/pages/swift-howto/). Example projects available in [docs/examples/objc](https://github.com/cossacklabs/themis/tree/master/docs/examples/objc) and [docs/examples/swift/](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) folders ([#432](https://github.com/cossacklabs/themis/pull/432), [#430](https://github.com/cossacklabs/themis/pull/430), [#428](https://github.com/cossacklabs/themis/pull/428), [#427](https://github.com/cossacklabs/themis/pull/427)).

  - Added BoringSSL support, now users can select which crypto-engine they want to include. This change affects only Themis CocoaPod: users can add Themis based on BoringSSL to their Podfile using `pod 'themis/themis-boringssl'` ([#351](https://github.com/cossacklabs/themis/pull/351), [#331](https://github.com/cossacklabs/themis/pull/331), [#330](https://github.com/cossacklabs/themis/pull/330), [#329](https://github.com/cossacklabs/themis/pull/329)).

  - Added bitcode support. This affects only Themis CocoaPod that uses OpenSSL – thanks [@deszip](https://github.com/deszip) and [@popaaaandrei](https://github.com/popaaaandrei) ([#407](https://github.com/cossacklabs/themis/pull/407), [#355](https://github.com/cossacklabs/themis/pull/355), [#354](https://github.com/cossacklabs/themis/pull/354)).

  - Added compatibility for Swift frameworks. Now Themis can be used directly from Swift without Bridging header file, kudos to [@popaaaandrei](https://github.com/popaaaandrei) for pointing on this out ([#416](https://github.com/cossacklabs/themis/pull/416), [#415](https://github.com/cossacklabs/themis/pull/415)).

  - Updated code to use the latest Secure Message API (see description of core changes above). This change doesn't affect user-facing code so no code changes are required from users ([#393](https://github.com/cossacklabs/themis/pull/393)).

  - Updated error codes and error messages for all crypto systems, now errors and logs are more user-friendly and understandable ([#394](https://github.com/cossacklabs/themis/pull/394), [#393](https://github.com/cossacklabs/themis/pull/393)).

  - Improved code quality here and there ([#317](https://github.com/cossacklabs/themis/pull/317)).

  - Dropped feature flag `SECURE_COMPARATOR_ENABLED` because it's redundant: Secure Comparator is enabled by default ([#429](https://github.com/cossacklabs/themis/pull/429)).

- **macOS specific**

  - Added Homebrew support for Themis Core. Now users can install Themis Core library using `brew tap cossacklabs/tap && brew update && brew install libthemis`. This is useful when you're developing on macOS.

  More details can be found in [the Installation guide](https://docs.cossacklabs.com/pages/documentation-themis/#macos).

- **C++**

  - Improved Secure Session memory behavior (now users can move and copy Secure Session objects and callbacks) ([#370](https://github.com/cossacklabs/themis/pull/370), [#369](https://github.com/cossacklabs/themis/pull/369)).

  - Allowed to link ThemisPP as header-only library by adding "inline" functions – thanks [@deszip](https://github.com/deszip) for pushing us. Check for detailed instructions in [C++ HowTo](https://docs.cossacklabs.com/pages/cpp-howto/) ([#371](https://github.com/cossacklabs/themis/pull/371)).

  - Added support of smart pointer constructors for Secure Session, now users should use `std::shared_ptr<secure_session_callback_interface_t>` constructor ([#378](https://github.com/cossacklabs/themis/pull/378)).

  - Added functions for key validation: now you can check if keypairs are valid before using it for encryption/decryption ([#389](https://github.com/cossacklabs/themis/pull/389)).

  - Updated test suite to test C++03 and C++11 ([#379](https://github.com/cossacklabs/themis/pull/379)).

  - Updated error codes and error messages for all crypto systems, now errors and logs are more user-friendly and understandable ([#385](https://github.com/cossacklabs/themis/pull/385)).

  - Formatted code using `clang-format` rules and implemented some `clang-tidy` recommendations ([#410](https://github.com/cossacklabs/themis/pull/410), [#404](https://github.com/cossacklabs/themis/pull/404)).

- **Java**

  - Updated Secure Message API: separated function `wrap` into `encrypt` and `sign`, and function `unwrap` into `decrypt` and `verify`. Old functions are still available, but will be deprecated eventually ([#389](https://github.com/cossacklabs/themis/pull/389)).

  - Significantly improved [Themis usage examples for Desktop Java](https://github.com/cossacklabs/themis-java-examples) - thanks to [@Dimdron](https://github.com/Dimdron) [#3](https://github.com/cossacklabs/themis-java-examples/pull/3).

  - Formatted JNI code using `clang-format` rules and implemented some `clang-tidy` recommendations ([#420](https://github.com/cossacklabs/themis/pull/420)).

- **Android**

  - Added Maven distribution ([#361](https://github.com/cossacklabs/themis/pull/361)).

    The new installation process requires adding only two lines to the Maven app configuration (instead of manually re-compiling the whole Themis library)!
    See the updated [HowTo guide](https://docs.cossacklabs.com/pages/java-and-android-howto/) in Wiki.

  - Significantly improved [Themis usage examples for Android](https://github.com/cossacklabs/themis-java-examples) - thanks to [@Dimdron](https://github.com/Dimdron) [#3](https://github.com/cossacklabs/themis-java-examples/pull/3).

  - Significantly improved [Secure mobile websocket example](https://github.com/cossacklabs/mobile-websocket-example) - thanks to [@sergeyzenchenko](https://github.com/sergeyzenchenko) [#4](https://github.com/cossacklabs/mobile-websocket-example/pull/4).

  - Formatted JNI code using `clang-format` rules and implemented some `clang-tidy` recommendations ([#420](https://github.com/cossacklabs/themis/pull/420)).

- **Go**

  - Updated code to use the latest Secure Message API (see the description of core changes above). This change doesn't affect user-facing code so no code changes are required from users ([#400](https://github.com/cossacklabs/themis/pull/400)).

  - Formatted code and fixed `gofmt` and `golint` warnings ([#426](https://github.com/cossacklabs/themis/pull/426), [#424](https://github.com/cossacklabs/themis/pull/424), [#432](https://github.com/cossacklabs/themis/pull/423), [#422](https://github.com/cossacklabs/themis/pull/422)).

- **Node.js**

  - Fixed jsthemis to be compatible with Node 10, huge thanks to [@deszip](https://github.com/deszip) ([#327](https://github.com/cossacklabs/themis/pull/327), [#326](https://github.com/cossacklabs/themis/pull/326)).

  - Updated error codes and error messages for all crypto systems, now errors and logs are more user-friendly and understandable ([#384](https://github.com/cossacklabs/themis/pull/384)).

  - Fixed memory corruption tests on i386 systems ([#408](https://github.com/cossacklabs/themis/pull/408)).

  - Formatted native extension code using `clang-format` rules and implemented some `clang-tidy` recommendations ([#412](https://github.com/cossacklabs/themis/pull/412)).

- **PHP**

  - Updated PHP installer to use the latest Composer installer ([#360](https://github.com/cossacklabs/themis/pull/360), [#328](https://github.com/cossacklabs/themis/pull/328)).

- **Python**

  - Updated code to use the latest Secure Message API (see description of core changes above). This change doesn't affect user-facing code so no code changes are required from users ([#401](https://github.com/cossacklabs/themis/pull/401)).

  - Updated error codes and error messages for all crypto systems, now errors and logs are more user-friendly and understandable ([#401](https://github.com/cossacklabs/themis/pull/401)).

- **Ruby**

  - Updated code to use latest Secure Message API (see description of core changes above). This change doesn't affect user-facing code so no code changes are required from users ([#402](https://github.com/cossacklabs/themis/pull/402)).

  - Updated error codes and error messages for all crypto systems, now errors and logs are more user-friendly and understandable ([#402](https://github.com/cossacklabs/themis/pull/402)).

  - Deprecated `rubythemis` in favor of `rbthemis`. Users should use `require 'rbthemis'` in their projects ([#434](https://github.com/cossacklabs/themis/pull/434)).

- **Tests and other things**

  - Added tools for fuzzing testing and tests on Themis Core ([#421](https://github.com/cossacklabs/themis/pull/421), [#368](https://github.com/cossacklabs/themis/pull/368), [#366](https://github.com/cossacklabs/themis/pull/366), [#364](https://github.com/cossacklabs/themis/pull/364)).

  - Updated BoringSSL submodule configuration to use Clang while building ([#352](https://github.com/cossacklabs/themis/pull/352)).

  - Updated NIST test suite: improved readability, maintainability, and output of NIST STS makefile, added build files to gitignore ([#414](https://github.com/cossacklabs/themis/pull/414)).


_Docs:_

- Described the new [Secure Message API](https://docs.cossacklabs.com/pages/secure-message-cryptosystem/#implementation-details): how we divided the `wrap` function into `encrypt` and `sign`, and the `unwrap` function — into `decrypt` and `verify` to make it more obvious for the users.

- Described [thread safety code practices](https://docs.cossacklabs.com/pages/thread-safety-themis/) when using Secure Session.

- Improved installation guides for numerous languages.


_Infrastructure:_

- Added Homebrew support for Themis Core. Now users can install Themis Core libraby using `brew tap cossacklabs/tap && brew update && brew install libthemis`. This is useful when you're developing on macOS. More details can be found in [the Installation guide](https://docs.cossacklabs.com/pages/documentation-themis/#macos).

- Added [installation guide on using Docker container](https://docs.cossacklabs.com/pages/documentation-themis/#themis-with-docker) as a building environment for Themis: if you can't download Themis Core from packages, feel free to use Docker container for this.


## [0.10.0](https://github.com/cossacklabs/themis/releases/tag/0.10.0), February 6th 2018

**TL;DR:** Multi-platform, multi-language compatibility improved.

⚠️ _Incompatibility issues:_

- If you are using Themis on x64 systems, consider upgrading every Themis library/wrapper you were using to `0.10.0`. Incompatibility issues may arise between previous Themis versions and `0.10.0` on x64 machines ([#279](https://github.com/cossacklabs/themis/pull/279)).

- Rubythemis has breaking changes for Secure Cell Token Protect mode ([#281](https://github.com/cossacklabs/themis/pull/281)). We added checks for other language wrappers to make sure this won't happen again ([#282](https://github.com/cossacklabs/themis/pull/282), [#283](https://github.com/cossacklabs/themis/pull/283)).

Check the [Migration Guide](https://docs.cossacklabs.com/pages/migration-guide-themis-v096-themis-0100/) for more details.

_Docs:_

- Updated the descriptions of [Crypto systems](https://docs.cossacklabs.com/pages/cryptosystems/), added more usage examples and code samples.
- Refreshed code samples in language tutorials, made them more readable.<br/>[Obj-C](https://docs.cossacklabs.com/pages/objective-c-howto/) | [Swift](https://docs.cossacklabs.com/pages/swift-howto/) | [Java and Android](https://docs.cossacklabs.com/pages/java-and-android-howto/) | [Python](https://docs.cossacklabs.com/pages/python-howto/) | [PHP](https://docs.cossacklabs.com/pages/php-howto/) | [Ruby](https://docs.cossacklabs.com/pages/ruby-howto/) | [C++](https://docs.cossacklabs.com/pages/cpp-howto/) | [Go](https://docs.cossacklabs.com/pages/go-howto/) | [Node.js](https://docs.cossacklabs.com/pages/nodejs-howto/).
- Added human-friendly description of [Secure Comparator](https://docs.cossacklabs.com/pages/secure-comparator-cryptosystem/) and supplied usage examples for all languages with the exception of PHP.

_Infrastructure:_

- Added support of _Ubuntu 17.10_.
- Removed support of _Ubuntu 16.10_ and _Ubuntu 17.04_ (no more compiled binaries for these OSs now).
- Added CLI utils for easy testing of Secure Cell, Secure Message, and Key generation on local machine. Available for Python, Ruby, Go, NodeJS, and PHP. Check the [Console Utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) guide for more details and usage description/guide.
- Added [Integration test suit](https://github.com/cossacklabs/themis/tree/master/tests/_integration) for the majority of the available language wrappers, which was the catalyst for many fixes in this release.
- Added support of CircleCI 2.0 with multiple workflows. This allows testing each language wrapper and integrations between them easily and quickly ([#295](https://github.com/cossacklabs/themis/pull/295), [#299](https://github.com/cossacklabs/themis/pull/299)).


_Code:_

- **Core:**
  - Fixed incompatibility issue that existed in Secure Cell between x32 and x64 machines ([#279](https://github.com/cossacklabs/themis/pull/279));
  - Added C examples for using buffer API in Secure Session ([#271](https://github.com/cossacklabs/themis/pull/271));
  - Fixed possible SigFault in Secure Message at the point of providing an incorrect private key path ([#286](https://github.com/cossacklabs/themis/pull/286)).
- **C++ wrapper:**
  - Added a set of unit tests ([#292](https://github.com/cossacklabs/themis/pull/292)).
- **Android wrapper**:
  - simplified Android build by providing a ready-to-use Docker container. No need to compile BoringSSL manually now! ([#273](https://github.com/cossacklabs/themis/pull/273) by [@secumod](https://github.com/secumod)).
- **iOS wrapper:**
  - Updated Secure Comparator definitions and provided code samples ([#287](https://github.com/cossacklabs/themis/pull/287), [#288](https://github.com/cossacklabs/themis/pull/288)).
- **GoThemis:**
  - Added `get_remote_id` function, which is making SecureSession easier to use ([#272](https://github.com/cossacklabs/themis/pull/272));
  - Added [CLI utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) and integration tests that allow you to test Secure Cell, Secure Message, and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **JSThemis:**
  - Added a separate installation step in Makefile. Now you can install jsthemis via `make jsthemis_install` ([#302](https://github.com/cossacklabs/themis/pull/302));
  - Added [CLI utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) and integration tests that allow you to test Secure Cell, Secure Message, and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **RubyThemis:**
  - Fixed arguments' order for Secure Cell in the Token Protect mode, which caused incompatibility with older versions of rubythemis ([#281](https://github.com/cossacklabs/themis/pull/281)). Please check the [migration guide](https://docs.cossacklabs.com/pages/migration-guide-themis-v096-themis-0100/) for the details;
  - Added [CLI utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) and integration tests that allow you to test Secure Cell, Secure Message, and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306))
- **PyThemis:**
  - Improved the installation process via Makefile for python3 users ([#300](https://github.com/cossacklabs/themis/pull/300));
  - Added [CLI utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) and integration tests that allow you to test Secure Cell, Secure Message, and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).
- **PHPThemis:**
  - Added support of PHP 7.0, 7.1, 7.2 ([#278](https://github.com/cossacklabs/themis/pull/278), [#280](https://github.com/cossacklabs/themis/pull/280));
  - Added a package for `phpthemis`. Now you don't need to compile it from sources. See the installation [PHP guide](https://docs.cossacklabs.com/pages/php-howto/#installing-stable-version-from-packages) for more details;
  - Improved [unit tests](https://docs.cossacklabs.com/pages/php-howto/#installing-stable-version-from-packages). Now it's easy to run tests because all the dependencies are handled by a php-composer ([#284](https://github.com/cossacklabs/themis/pull/284), [#285](https://github.com/cossacklabs/themis/pull/285), [#303](https://github.com/cossacklabs/themis/pull/303));
  - Added a memory test suit, which allows us to keep a closer eye on PHPThemis' memory usage ([#298](https://github.com/cossacklabs/themis/pull/298));
  - Added [CLI utils](https://docs.cossacklabs.com/pages/documentation-themis/#encrypt-decrypt-console-utils) and integration tests that allow you to test Secure Cell, Secure Message, and Key generation on your local machine ([#277](https://github.com/cossacklabs/themis/pull/277), [#293](https://github.com/cossacklabs/themis/pull/293), [#305](https://github.com/cossacklabs/themis/pull/305), [#306](https://github.com/cossacklabs/themis/pull/306)).


## [0.9.6](https://github.com/cossacklabs/themis/releases/tag/0.9.6), December 14th 2017

**TL;DR:** OpenSSL 1.1 support.

_Docs:_

- Significant update of the [Contributing section](https://docs.cossacklabs.com/pages/documentation-themis/#contributing-to-themis).

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
* Enhanced **[documentation](https://docs.cossacklabs.com/products/themis/)**
* Lots of various demo projects
* Updated **[Themis Server](https://themis.cossacklabs.com)**
* Better **make** system verbosity (now you can actually see what succeeded and what didn't)
* Infrastructure to **build Java** on all platforms

_Code_:
* **iOS wrapper** now has umbrella header.
* We added **Swift** language [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/swift) and [howto](https://docs.cossacklabs.com/pages/swift-howto/).
* Themis wrapper for **Go** language: [howto](https://docs.cossacklabs.com/pages/go-howto/) (examples coming soon).
* Themis wrapper for **NodeJS**: [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/nodejs) and [howto](https://docs.cossacklabs.com/pages/nodejs-howto/).
* Google Chrome-friendly spin-off called [WebThemis](https://github.com/cossacklabs/webthemis) was released.
* Themis wrapper for **C++**: [examples](https://github.com/cossacklabs/themis/tree/master/docs/examples/c%2B%2B) and [HowTo](https://docs.cossacklabs.com/pages/cpp-howto/).
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
