# Themis ChangeLog

## Unreleased

Changes that are currently in development and have not been released yet.

## [0.15.0](https://github.com/cossacklabs/themis/releases/tag/0.15.0), June 21st 2023

**TL;DR:**

- Uncompressed EC public keys are now supported.
- Increased PBKDF2 iteration count from 200000 to 314110 for Secure Cell passphrase mode.
- OpenSSL 3.0 is now supported.
- Pythemis now uses `pyproject.toml`.
- And as usual: enhanced security measures and fixed bugs.

**Breaking changes and deprecations:**
- AndroidThemis build requires Gradle 7.3, Android SDK 11, Android NDK 25.
- Some Soter functions are deprecated.
- Node.js 8 is no longer supported.
- Rust `SecureSessionTransport` implementations are now `Send`.
- Rust 1.58 is now the minimum supported version.

_Code:_

- **Core**

  - Uncompressed EC public keys are now supported ([#959](https://github.com/cossacklabs/themis/pull/959), [#954](https://github.com/cossacklabs/themis/pull/954))
  - Themis will generate uncompressed EC public keys when `THEMIS_GEN_EC_KEY_PAIR_UNCOMPRESSED=1` environment variable is set ([#959](https://github.com/cossacklabs/themis/pull/959))
  - Increased PBKDF2 iteration count to maintain security of Secure Cell passphrase mode ([#976](https://github.com/cossacklabs/themis/pull/976)).
  - Bumped embedded BoringSSL to the latest version ([#1004](https://github.com/cossacklabs/themis/pull/1004)).

  - **Soter** (low-level security core used by Themis)

    - `soter_sign_export_key()` is now deprecated, superseded by `soter_sign_export_private_key()` and `soter_sign_export_public_key()` ([#959](https://github.com/cossacklabs/themis/pull/959))
    - better OpenSSL 3 support, with many EC- and RSA-related deprecated functions being replaced with newer alternatives, and OpenSSL 1.X is still supported
    - removed build option THEMIS_EXPERIMENTAL_OPENSSL_3_SUPPORT since building/linking with OpenSSL 3 now works out of the box

- **Android**

  - AndroidThemis build from source now requires Gradle 7.3, Android SDK 11, Android NDK 25 ([#942](https://github.com/cossacklabs/themis/pull/942)).

- **Node.js**

  - Node.js v8 is no longer supported ([#901](https://github.com/cossacklabs/themis/pull/901)).
  - Fixed bug that leads to segfauls if key pair generation fails ([#999](https://github.com/cossacklabs/themis/pull/999))

- **Python**

  - `pythemis.scomparator` and `pythemis.skeygen` are now imported with `from pythemis import *` ([#914](https://github.com/cossacklabs/themis/pull/914)).
  - Pythemis supports `pyproject.toml` as a main way of building packages. The old `setup.py` is preserved for backwards compatibility ([#1006](https://github.com/cossacklabs/themis/pull/1006)).

- **Ruby**

  - Improved compatibility with non-standard installations on Apple M1 ([#917](https://github.com/cossacklabs/themis/pull/917)).

- **Rust**

  - `SecureSessionTransport` implementations are now required to be `Send` ([#898](https://github.com/cossacklabs/themis/pull/898)).

    This is technically a breaking change, but most reasonble implementations should be `Send` already. Please raise an issue if your code fails to build.

  - Minimum supported Rust version is now 1.58 ([#977](https://github.com/cossacklabs/themis/pull/977), [#984](https://github.com/cossacklabs/themis/pull/984)).
  - Bindgen is pinned to 0.66.1 on CI ([#1008](https://github.com/cossacklabs/themis/pull/1008)).

- **WebAssembly**

  - Node.js v8 is no longer supported ([#901](https://github.com/cossacklabs/themis/pull/901)).


## [0.14.10](https://github.com/cossacklabs/themis/releases/tag/0.14.10), Nov 3rd 2022

### React Native Themis

- Redesigned the framework to make it less vulnerable to native exceptions ([#955](https://github.com/cossacklabs/themis/pull/955)).

- Updates for react native examples. ([#956](https://github.com/cossacklabs/themis/pull/956)).

### PyThemis

- Improved compatibility with non-Homebrew Python installations on Apple M1 ([#915](https://github.com/cossacklabs/themis/pull/915)).


## [0.14.9](https://github.com/cossacklabs/themis/releases/tag/0.14.9), Sep 12th 2022

### Hotfix for macOS Themis installed via SPM

- Fixing the code signing issue for macOS Themis installation via SPM ([#944](https://github.com/cossacklabs/themis/pull/944/)).




## [0.14.8](https://github.com/cossacklabs/themis/releases/tag/0.14.8), Aug 22th 2022

### WASM Themis

- Rebuild wasm-themis with optimization flag to make package smaller again.
([938](https://github.com/cossacklabs/themis/pull/938/)).


## [0.14.7](https://github.com/cossacklabs/themis/releases/tag/0.14.7), Aug 12th 2022

### React Native Themis

**Upgrade packages versions for security reasons in React Native Themis package.json
([934](https://github.com/cossacklabs/themis/pull/934/)).**

**Resolve the issue in React Native Themis to support Javascript exceptions
([930](https://github.com/cossacklabs/themis/pull/930/))**

- Define additional checks to generate Javascript exceptions in case of empty required parameters.

### WASM Themis

**Fix the bug with encryption of large files
([933](https://github.com/cossacklabs/themis/pull/933/)).**

- Added option to build WASM file.


## [0.14.6](https://github.com/cossacklabs/themis/releases/tag/0.14.6), May 24th 2022

**Reduce WasmThemis binary size x3, by increasing the compiler optimization level for WasmThemis build ([919](https://github.com/cossacklabs/themis/pull/919/)).**

## [0.14.5](https://github.com/cossacklabs/themis/releases/tag/0.14.5), April 14th 2022

**Typescript compatibility issues in WasmThemis**

- Improved SecureCell Seal in WasmThemis. Added optional context parameters to SecureCell Seal mode instead of using arguments in methods `encrypt()` and `decrypt()` to make compilers happy ([#909]).

[#909]: https://github.com/cossacklabs/themis/pull/909

## [0.14.4](https://github.com/cossacklabs/themis/releases/tag/0.14.4), March 15th 2022 / April 05th 2022

**React Native Support**

- Added React Native Themis with Typescript support ([#902], [#903], [#905], [#906]).

  [`react-native-themis`](https://www.npmjs.com/package/react-native-themis) is now available on npm! 🎉

      npm install react-native-themis

  [Read more about React Native support](https://docs.cossacklabs.com/themis/languages/react-native/).

[#902]: https://github.com/cossacklabs/themis/pull/902
[#903]: https://github.com/cossacklabs/themis/pull/903
[#905]: https://github.com/cossacklabs/themis/pull/905
[#906]: https://github.com/cossacklabs/themis/pull/906


## [0.14.0](https://github.com/cossacklabs/themis/releases/tag/0.14.0), December 24th 2021

**TL;DR:**

- JavaThemis for Android has moved to Maven Central ([read more](#0.14-maven-central)).
- JavaThemis for Java is available on Maven Central ([read more](#0.14-maven-central)).
- Themis is now available via Swift Package Manager ([read more](https://docs.cossacklabs.com/themis/languages/swift/installation/#installing-with-spm))
- Themis for iOS and macOS is also available as XCFramework, attached to the release.
- TypeScript definitions and ES6 module are now available for WasmThemis ([read more](#0.14-typescript)).
- Node.js v16 is now supported.
- [Overhaul of documentation once again](https://docs.cossacklabs.com/themis/).
- New Themis Core packages with BoringSSL – `libthemis-boringssl`.
- OpenSSL 3.0 support is work-in-progress, but not ready yet ([read more](https://github.com/cossacklabs/themis/issues/873)).
- And as usual: more security hardening, fewer known bugs.

**Breaking changes and deprecations:**

- Themis Core: private C header files are no longer installed.
- GoThemis: `ErrOverflow` is now deprecated.
- Rust: `themis` now requires Rust 1.47 or newer.
- Rust: `libthemis-src` is no longer supported.
- Installing Themis via Carthage now requires Carthage 0.38 or newer ([read more](#0.13.9-carthage-0.38)).
- Deprecated CocoaPods subspecs `themis/themis-openssl` and `themis/themis-boringssl` have been removed ([read more](#0.13.10-subspecs))
- Themis Server (Themis Interactive Simulator) is no longer supported ([read more](https://docs.cossacklabs.com/themis/debugging/themis-server/).

_Code:_

- **Core**

  - Include embedded BoringSSL into Soter for convenience ([#681](https://github.com/cossacklabs/themis/pull/681), [#702](https://github.com/cossacklabs/themis/pull/702)).
  - `make deb` and `make rpm` with `ENGINE=boringssl` will now produce `libthemis-boringssl` packages with embedded BoringSSL ([#683](https://github.com/cossacklabs/themis/pull/683), [#686](https://github.com/cossacklabs/themis/pull/686)).
  - `secure_session_create()` now allows only EC keys, returning an error for RSA ([#693](https://github.com/cossacklabs/themis/pull/693)).
  - Cleaned up unused private API. Thanks to [**@luismerino**](https://github.com/luismerino) for pointing this out ([#714](https://github.com/cossacklabs/themis/pull/714)).
  - Cleaned up public header files and API of Themis and Soter ([#759](https://github.com/cossacklabs/themis/pull/759)).

    Private header files are no longer installed. Private APIs which have been unintentially exported are no longer available. This might be a **breaking change** for those who have used them. Please refrain from using private API and include only public API:

    ```c
    #include <themis/themis.h>
    ```

    Users of official high-level wrappers are not affected. However, this might affect developers of third-party wrappers. Refer to the detailed description below for a list of removed headers.

    <details>

    The following Soter headers are no longer available:

      - `<soter/soter_container.h>`
      - `<soter/soter_crc32.h>`
      - `<soter/soter_ec_key.h>`
      - `<soter/soter_portable_endian.h>`
      - `<soter/soter_rsa_key.h>`
      - `<soter/soter_sign_ecdsa.h>`
      - `<soter/soter_sign_rsa.h>`
      - `<soter/soter_t.h>`

    All APIs previously exported by them are no longer available as well.

    The following Themis headers are no longer available:

      - `<themis/secure_cell_alg.h>`
      - `<themis/secure_cell_seal_passphrase.h>`
      - `<themis/secure_comparator_t.h>`
      - `<themis/secure_message_wrapper.h>`
      - `<themis/secure_session_peer.h>`
      - `<themis/secure_session_t.h>`
      - `<themis/secure_session_utils.h>`
      - `<themis/sym_enc_message.h>`
      - `<themis/themis_portable_endian.h>`

    All APIs previously exported by them are no longer available as well.

    In addition to that, the following private symbols and definitions previously exported by `<themis/secure_session.h>` have been hidden:

      - `THEMIS_SESSION_ID_TAG`
      - `THEMIS_SESSION_PROTO_TAG`
      - `SESSION_MASTER_KEY_LENGTH`
      - `SESSION_MESSAGE_KEY_LENGTH`
      - `struct secure_session_peer_type`
      - `typedef secure_session_peer_t`
      - `typedef secure_session_handler`
      - `secure_session_peer_init()`
      - `secure_session_peer_cleanup()`

    </details>

  - Fixed multiple buffer overflows in Secure Message ([#763](https://github.com/cossacklabs/themis/pull/763)).
  - Fixed cross-compilation on macOS by setting `ARCH` and `SDK` variables ([#849](https://github.com/cossacklabs/themis/pull/849)).
  - Updated embedded BoringSSL to the latest version ([#812](https://github.com/cossacklabs/themis/pull/812)).
  - Builds with OpenSSL 3.0 will result in a compilation error for the time being ([#872](https://github.com/cossacklabs/themis/pull/872)).
  - Hardened EC/RSA key generation and handling in Secure Message and Secure Session ([#875](https://github.com/cossacklabs/themis/pull/875), [#876](https://github.com/cossacklabs/themis/pull/876))

- **Android**

  - Example project moved to the main repository – [`docs/examples/android`](docs/examples/android) ([#813](https://github.com/cossacklabs/themis/pull/813)).
  - Example project is now written in Kotlin ([#813](https://github.com/cossacklabs/themis/pull/813)).
  - Updated embedded BoringSSL to the latest version ([#812](https://github.com/cossacklabs/themis/pull/812)).

- **C++**

  - `themispp::secure_message_t::sign()` output is a bit smaller now ([#775](https://github.com/cossacklabs/themis/pull/775)).

- **Go**

  - Error `ErrOverflow` is now deprecated in favor of `ErrOutOfMemory`, new error types were added ([#711](https://github.com/cossacklabs/themis/pull/711)).
  - `SecureMessage.Sign()` output is a bit smaller now ([#775](https://github.com/cossacklabs/themis/pull/775)).

- **Java / Kotlin**

  - `SecureMessage#sign()` output is a bit smaller now ([#777](https://github.com/cossacklabs/themis/pull/777)).
  - <a id="0.14-maven-central">JavaThemis for Android and desktop Java is now published in the Maven Central repository</a> ([#786](https://github.com/cossacklabs/themis/pull/786), [#788](https://github.com/cossacklabs/themis/pull/788)).

    Add the Maven Central repository to your `build.gradle`:

    ```groovy
    repositories {
        mavenCentral()
    }
    ```

    For Android, use this dependency:

    ```groovy
    dependencies {
        implementation 'com.cossacklabs.com:themis:0.14.0'
    }
    ```

    For desktop systems use this one:

    ```groovy
    dependencies {
        implementation 'com.cossacklabs.com:java-themis:0.14.0'
    }
    ```

  - Example project for desktop Java moved to the main repository – [`docs/examples/java`](docs/examples/java) ([#816](https://github.com/cossacklabs/themis/pull/816)).

- **Objective-C**

  - Updated Objective-C examples (iOS and macOS, Carthage and CocoaPods) to showcase usage of the newest Secure Cell API: generating symmetric keys and using Secure Cell with Passphrase ([#688](https://github.com/cossacklabs/themis/pull/688)) and to use latest Themis 0.13.4 ([#701](https://github.com/cossacklabs/themis/pull/701), [#703](https://github.com/cossacklabs/themis/pull/703), [#706](https://github.com/cossacklabs/themis/pull/706), [#723](https://github.com/cossacklabs/themis/pull/723), [#724](https://github.com/cossacklabs/themis/pull/724), [#726](https://github.com/cossacklabs/themis/pull/726), [#740](https://github.com/cossacklabs/themis/pull/740)).
  - `TSSession` initializer now returns an error (`nil`) when given incorrect key type ([#710](https://github.com/cossacklabs/themis/pull/710)).
  - Improved compatibility with Xcode 12 ([#742](https://github.com/cossacklabs/themis/pull/742)).
  - Updated CocoaPods examples to the latest Themis version 0.13.10 ([#834](https://github.com/cossacklabs/themis/pull/834)).
  - Removed deprecated CocoaPods subspecs: `themis/themis-openssl` and `themis/themis-boringssl` ([#884](https://github.com/cossacklabs/themis/pull/884), [#885](https://github.com/cossacklabs/themis/pull/885)).

- **PHP**

  - `libphpthemis` packages for Debian/Ubuntu now have accurate dependencies ([#683](https://github.com/cossacklabs/themis/pull/683)).
  - PHP Composer 2.0 is now supported by PHPThemis unit tests ([#730](https://github.com/cossacklabs/themis/pull/730)).

- **Node.js**

  - `SecureSession` constructor now throws an exception when given incorrect key type ([#698](https://github.com/cossacklabs/themis/pull/698)).
  - Node.js v16 is now supported ([#801](https://github.com/cossacklabs/themis/pull/801)).

- **Python**

  - `SSession` constructor now throws an exception when given incorrect key type ([#710](https://github.com/cossacklabs/themis/pull/710)).

- **Ruby**

  - `Ssession` constructor now throws an exception when given incorrect key type ([#710](https://github.com/cossacklabs/themis/pull/710)).

- **Rust**

  - Dropped `libthemis-src` crate support and removed the `vendored` feature. RustThemis wrapper now requires Themis Core to be installed in the system ([#691](https://github.com/cossacklabs/themis/pull/691)).
  - Updated `zeroize` depedency to 1.x version. Rust 1.47 or newer is now required ([#799](https://github.com/cossacklabs/themis/pull/799)).

- **Swift**

  - Updated Swift examples (iOS and macOS, Carthage and CocoaPods) to showcase usage of the newest Secure Cell API: generating symmetric keys and using Secure Cell with Passphrase ([#688](https://github.com/cossacklabs/themis/pull/688)) and to use latest Themis 0.13.4 ([#701](https://github.com/cossacklabs/themis/pull/701), [#703](https://github.com/cossacklabs/themis/pull/703), [#706](https://github.com/cossacklabs/themis/pull/706), [#740](https://github.com/cossacklabs/themis/pull/740)).
  - `TSSession` initializer now returns an error (`nil`) when given incorrect key type ([#710](https://github.com/cossacklabs/themis/pull/710)).
  - Improved compatibility with Xcode 12 ([#742](https://github.com/cossacklabs/themis/pull/742)).
  - Updated CocoaPods examples to the latest Themis version 0.13.10 ([#834](https://github.com/cossacklabs/themis/pull/834)).
  - Removed deprecated CocoaPods subspecs: `themis/themis-openssl` and `themis/themis-boringssl` ([#884](https://github.com/cossacklabs/themis/pull/884), [#885](https://github.com/cossacklabs/themis/pull/885)).

- **WebAssembly**

  - Updated Emscripten toolchain to the latest version ([#760](https://github.com/cossacklabs/themis/pull/760), [#880](https://github.com/cossacklabs/themis/pull/880)).
  - Node.js v16 is now supported ([#801](https://github.com/cossacklabs/themis/pull/801)).
  - <a id="0.14-typescript">TypeScript type definitions and ES6 module are now available,</a> thanks to [**@maxammann**](https://github.com/maxammann) ([#792](https://github.com/cossacklabs/themis/pull/792)).
    ```js
    import {SecureCell, SecureMessage, SecureSession, SecureComparator} from 'wasm-themis';
    ```
  - New initialization API: `initialize()`, allowing to specify custom URL for `libthemis.wasm` ([#792](https://github.com/cossacklabs/themis/pull/792), [#854](https://github.com/cossacklabs/themis/pull/854), [#857](https://github.com/cossacklabs/themis/pull/857)).
  - Updated embedded BoringSSL to the latest version ([#812](https://github.com/cossacklabs/themis/pull/812)).
  - `make wasmthemis` now fails with unsupported Emscripten toolchains ([#879](https://github.com/cossacklabs/themis/pull/879)).

_Infrastructure:_

- Improved package split making `libthemis` thinner ([#678](https://github.com/cossacklabs/themis/pull/678)).
- Optimized dependencies of `libthemis` DEB and RPM packages ([#682](https://github.com/cossacklabs/themis/pull/682), [#686](https://github.com/cossacklabs/themis/pull/686)).
- `make deb` and `make rpm` with `ENGINE=boringssl` will now produce `libthemis-boringssl` packages with embedded BoringSSL ([#683](https://github.com/cossacklabs/themis/pull/683), [#686](https://github.com/cossacklabs/themis/pull/686)).
- Build system and tests now respect the `PATH` settings ([#685](https://github.com/cossacklabs/themis/pull/685)).
- Rename embedded BoringSSL symbols by default to avoid conflicts with system OpenSSL ([#702](https://github.com/cossacklabs/themis/pull/702)).
- Started phasing out CircleCI in favour of GitHub Actions ([#709](https://github.com/cossacklabs/themis/pull/709), [#755](https://github.com/cossacklabs/themis/pull/755)).
- Themis is now fuzzed with `afl++` ([#766](https://github.com/cossacklabs/themis/pull/766)).
- Secure Message is now covered with fuzz testing ([#762](https://github.com/cossacklabs/themis/pull/762)).
- JavaThemis for Android and desktop Java is now published in the Maven Central repository ([#786](https://github.com/cossacklabs/themis/pull/786), [#788](https://github.com/cossacklabs/themis/pull/788)).
- MSYS2 builds for Windows are now checked by CI ([#791](https://github.com/cossacklabs/themis/pull/791)).
- Added automated tests for Android example project ([#813](https://github.com/cossacklabs/themis/pull/813)).
- Added automated tests for desktop Java example project ([#816](https://github.com/cossacklabs/themis/pull/816)).
- Embedded BoringSSL now builds faster if Ninja is available ([#837](https://github.com/cossacklabs/themis/pull/837)).
- Embedded BoringSSL can now be cross-compiled on macOS by setting `ARCH` and `SDK` variables ([#849](https://github.com/cossacklabs/themis/pull/849)).
- Builds on macOS use OpenSSL 1.1 from Homebrew by default ([#871](https://github.com/cossacklabs/themis/pull/871)).
- Builds with OpenSSL 3.0 are currently **not supported** ([#872](https://github.com/cossacklabs/themis/pull/872)).


## [0.13.12](https://github.com/cossacklabs/themis/releases/tag/0.13.12), July 26th 2021

**Hotfix for Apple arm64 simulators for M1**

- Fixed issue [864](https://github.com/cossacklabs/themis/issues/864): Themis XCFramework now includes arm64 slice for simulators ([865](https://github.com/cossacklabs/themis/pull/865)).

_Code:_

  - Fixed `Themis.xcodeproj` build settings: removed arm64 from exluded architectures ([865](https://github.com/cossacklabs/themis/pull/865)).


## [0.13.11](https://github.com/cossacklabs/themis/releases/tag/0.13.11), July 6th 2021

**Hotfix for WasmThemis:**

- Fixed issue when bundling WasmThemis with webpack ([#779](https://github.com/cossacklabs/themis/issue/779)).

_Code:_

- **WebAssembly**

  - Fixed issue with `TypeError: TextEncoder is not a constructor` when bundling WasmThemis with webpack ([#779](https://github.com/cossacklabs/themis/issue/779)).


## [0.13.10](https://github.com/cossacklabs/themis/releases/tag/0.13.10), May 26th 2021

<a id="0.13.10-subspecs"></a>
**Deprecation Notice for CocoaPods users:**
  - `themis/themis-openssl` subspec based on GRKOpenSSLFramework is deprecated and will be removed in Themis version 0.14.
  - `themis/themis-boringssl` subspec based on BoringSSL is deprecated and will be removed in Themis version 0.14.

Please, switch to the default option in your Podfile: `pod 'themis'`

**Hotfix for Apple platforms:**

- `themis` for CocoaPods now uses XCFrameworks, supports Apple Silicon, and OpenSSL 1.1.1k ([#828](https://github.com/cossacklabs/themis/pull/828)).
- Updated Carthage examples to use Themis XCFramework ([#823](https://github.com/cossacklabs/themis/pull/823)).

_Code:_

- **Objective-C / Swift**

  - `themis` for CocoaPods now uses XCFrameworks, supports Apple Silicon, and OpenSSL 1.1.1k ([#828](https://github.com/cossacklabs/themis/pull/828)).

## [0.13.9](https://github.com/cossacklabs/themis/releases/tag/0.13.9), May 14th 2021

**Hotfix for Apple platforms:**

- `themis` for Carthage switched to using XCFrameworks ([#817](https://github.com/cossacklabs/themis/pull/817)). So, <a id="0.13.9-carthage-0.38">the minimum required Carthage version</a> is now [0.38.0](https://github.com/Carthage/Carthage/releases/tag/0.38.0). You can continue using previous Themis version with previous Carthage versions.
- Updated OpenSSL to the latest 1.1.1k for Carthage ([#817](https://github.com/cossacklabs/themis/pull/817)).

_Code:_

- **Objective-C / Swift**

  - `themis` for Carthage now pulls OpenSSL dependency as XCFramework, and Carthage builds `themis` as XCFramework as well. `Themis.xcodeproj` now uses `openssl.xcframwork` and `themis.xcframework`. Carthage dependencies should be built with `--use-xcframeworks` flag ([#817](https://github.com/cossacklabs/themis/pull/817)).
  - Updated OpenSSL to the latest 1.1.1k for Carthage ([#817](https://github.com/cossacklabs/themis/pull/817)).
  - Tests (Github Actions) are updated to use the latest Carthage version (0.38.0 and up) and `--use-xcframeworks` flag ([#817](https://github.com/cossacklabs/themis/pull/817)).

## [0.13.8](https://github.com/cossacklabs/themis/releases/tag/0.13.8), April 30th 2021

**Hotfix for Apple platforms:**

- Updated OpenSSL to the latest 1.1.1k for SPM and attached `themis.xcframework` (iOS and macOS) ([#808](https://github.com/cossacklabs/themis/pull/808)).
- New Swift and Objective-C example projects: SPM for iOS and macOS ([#808](https://github.com/cossacklabs/themis/pull/808)).

_Code:_

- **Objective-C / Swift**

  - Updated OpenSSL to the latest 1.1.1k for SPM and attached `themis.xcframework`. It is `openssl-apple` version 1.1.11101 ([#808](https://github.com/cossacklabs/themis/pull/808)).
  - New Swift and Objective-C example projects: SPM for iOS and macOS ([#808](https://github.com/cossacklabs/themis/pull/808)).
  - Updated SPM examples source code to remove deprecated calls ([#808](https://github.com/cossacklabs/themis/pull/808)).

## [0.13.7](https://github.com/cossacklabs/themis/releases/tag/0.13.7), April 28th 2021

**Hotfix for Apple platforms:**

- `themis` is now packaged as XCFramework ([#789](https://github.com/cossacklabs/themis/pull/789)). It is available in the release attached files section.
- `themis` now supports SPM ([#789](https://github.com/cossacklabs/themis/pull/789)), its installation and usage are very straightforward, just add `themis` as SPM dependency.

_Code:_

- **Objective-C / Swift**

  - Added script to generate XCFramework for iOS, iOS Simulator and macOS ([#789](https://github.com/cossacklabs/themis/pull/789)).
  - Added Package.swift file for SPM ([#789](https://github.com/cossacklabs/themis/pull/789)).



## [0.13.6](https://github.com/cossacklabs/themis/releases/tag/0.13.6), November 23rd 2020

**Hotfix for Apple platforms:**

- `themis` pod is now restored to use *dynamic* linkage again
- Resolved errors related to `@rpath` when using CocoaPods
- Removed `arm64e` architecture slice from Carthage builds for iOS which prevented CocoaPods from functioning correctly

_Code:_

- **Objective-C / Swift**

  - CocoaPods will now again link ObjCThemis *dynamically* into application ([#750](https://github.com/cossacklabs/themis/pull/750)).
  - Carthage no longer builds `arm64e` architecture slice ([#750](https://github.com/cossacklabs/themis/pull/750)).
  - Updated OpenSSL to the latest 1.1.1h-2 ([#750](https://github.com/cossacklabs/themis/pull/750)).


## [0.13.5](https://github.com/cossacklabs/themis/releases/tag/0.13.5), November 12th 2020

**Hotfix for Apple platforms:**

- Resolved issues with deploying macOS apps with Apple Silicon support when Themis is installed via Carthage

_Code:_

- **Objective-C / Swift**

  - Fixed code signing issues on macOS with Xcode 12 ([#744](https://github.com/cossacklabs/themis/pull/744)).


## [0.13.4](https://github.com/cossacklabs/themis/releases/tag/0.13.4), October 29th 2020

**Hotfix for Apple platforms:**

- Improved Apple Silicon support (arm64 builds for macOS with Xcode 12.2 beta)
- Resolved issues with stable Xcode 12 support (disabled arm64 builds for iOS Simulator)
- Updated OpenSSL to the latest 1.1.1h
- CocoaPods is now using OpenSSL 1.1.1h by default (again)
- CocoaPods and Carthage now both produce full-static builds of Themis, resolving critical issues with App Store deployment (see [#715](https://github.com/cossacklabs/themis/issues/715))

_Code:_

- **Objective-C / Swift**

  - Switched to test on Xcode 12.0, disable ARM64 builds for Themis CocoaPods and Themis Carthage ([#721](https://github.com/cossacklabs/themis/pull/721), [#722](https://github.com/cossacklabs/themis/pull/722), [#732](https://github.com/cossacklabs/themis/pull/732), [#733](https://github.com/cossacklabs/themis/pull/733)).
  - CocoaPods will now link ObjCThemis statically into application ([#731](https://github.com/cossacklabs/themis/pull/731), [#735](https://github.com/cossacklabs/themis/pull/735)).
  - Updated OpenSSL to the latest 1.1.1h ([#735](https://github.com/cossacklabs/themis/pull/735)).


## [0.13.3](https://github.com/cossacklabs/themis/releases/tag/0.13.3), October 12th 2020

**Hotfix for Themis CocoaPods and Xcode12:**

- Default Themis podspec is using OpenSSL 1.0.2u again ("themis/themis-openssl"). OpenSSL 1.1.1g podspec ("themis/openssl") might be broken for Xcode12, fixing is in progress. BoringSSL podspec ("themis/themis-boringssl") is available too.

_Code:_

- **Objective-C / Swift**

  - Themis CocoaPods podspec is updated with bitcode fixes and disabling arm64 simulator in order to support Xcode12 builds. This is a podspec change only, no changes in code, headers or whatsoever. Default podspec is set as "themis/themis-openssl", which uses OpenSSL 1.0.2u. Fixes for "themis/openssl" podspec (OpenSSL 1.1.1g) might arrive soon.


## [0.13.2](https://github.com/cossacklabs/themis/releases/tag/0.13.2), August 14th 2020

**Breaking changes and deprecations:**

- ObjCThemis framework built by Carthage is now called `themis.framework` once again ([read more](#0.13.2-revert-objcthemis-rename)).

_Code:_

- **Objective-C**

  - **Breaking changes**

    - <a id="0.13.2-revert-objcthemis-rename">ObjCThemis framework built by Carthage is now called `themis.framework` once again</a> ([#704](https://github.com/cossacklabs/themis/pull/704)).

      [ObjCThemis 0.13.0](#0.13.0-objcthemis-rename) has initiated renaming of the framework produced by Carthage into `objcthemis.framework` from its historical name `themis.framework`. This decision has been a mistake. More information is available in the pull request linked above.

      `objcthemis.framework` is removed and should not be used.

      Please continue linking your applications to `themis.framework`. Note as well that starting with ObjCThemis 0.13.1, you do not have to embed `openssl.framework` anymore when ObjCThemis is installed via Carthage.

## [0.13.1](https://github.com/cossacklabs/themis/releases/tag/0.13.1), August 13th 2020

**TL;DR:**

- AndroidThemis is now available on JCenter
- ObjCThemis and SwiftThemis get latest OpenSSL update
- ObjCThemis and SwiftThemis now require Xcode 11 (or later)
- Minor security fixes in GoThemis, JsThemis, WasmThemis

_Code:_

- **Core**

  - Improved compatibility with OpenSSL 1.1.1 ([#684](https://github.com/cossacklabs/themis/pull/684)).

- **Android**

  - AndroidThemis is now available on JCenter ([#679](https://github.com/cossacklabs/themis/pull/679)).

- **Go**

  - Fixed panics on 32-bit systems when processing corrupted data ([#677](https://github.com/cossacklabs/themis/pull/677)).
  - Improved GoThemis package README and documentation ([#699](https://github.com/cossacklabs/themis/pull/699)).

- **Node.js**

  - Minor dependency updates making the world a better place ([#680](https://github.com/cossacklabs/themis/pull/680)).

- **WebAssembly**

  - Minor dependency updates making the world a better place ([#680](https://github.com/cossacklabs/themis/pull/680)).

- **Swift**

  - Updated OpenSSL to the latest 1.1.1g ([#692](https://github.com/cossacklabs/themis/pull/692)).

- **Objective-C**

  - Updated OpenSSL to the latest 1.1.1g ([#692](https://github.com/cossacklabs/themis/pull/692)).

_Infrastructure:_

- AndroidThemis is now available on JCenter ([#679](https://github.com/cossacklabs/themis/pull/679)).
- ObjCThemis and SwiftThemis now require Xcode 11 ([#692](https://github.com/cossacklabs/themis/pull/692)).

## [0.13.0](https://github.com/cossacklabs/themis/releases/tag/0.13.0), July 8th 2020

**TL;DR:**

- Added API for generating symmetric keys for use with Secure Cell.
- Added API for Secure Cell encryption with human-readable passphrases.
- [New supported platforms: Kotlin, Swift 5, iPadOS](#0.13.0-new-platforms).
- [Updated look-and-feel of the documentation](https://docs.cossacklabs.com/products/themis/).
- Squished many tricky bugs and added whole lot of extra security checks.

**Breaking changes and deprecations:**

- Many languages received Secure Cell API overhaul with parts of the old API becoming deprecated. Refer to individual language sections for details.
- ObjCThemis installed via Carthage is now called `objcthemis` instead of just `themis` ([read more](#0.13.0-objcthemis-rename)).
- Themis 0.9.6 compatibility is now disabled by default ([read more](#0.13.0-drop-0.9.6-compat)).
- Themis is known to be broken on big-endian architectures ([read more](#0.13.0-big-endian)).
- Java 7 is no longer supported, breaking Android and Java builds on outdated systems ([read more](#0.13.0-drop-java-7)).
- Python 2 is no longer supported ([read more](#0.13.0-drop-python-2)).
- Serialisation of Secure Session state in JavaThemis is now deprecated
  ([read more](#0.13.0-deprecate-session-save-restore)).

_Code:_

- **Core**

  - Added support for building with sanitizers like ASan and UBSan,
    enabled by `WITH_ASAN=1` flags
    ([#548](https://github.com/cossacklabs/themis/pull/548),
     [#556](https://github.com/cossacklabs/themis/pull/556)).
  - Fixed a number of possible use-after-free conditions
    ([#546](https://github.com/cossacklabs/themis/pull/546)).
  - Themis Core is now compiled with `-O2` optimizations enabled by default
    ([#543](https://github.com/cossacklabs/themis/pull/543)).
  - Themis Core is now compiled with even more paranoid compiler flags
    ([#578](https://github.com/cossacklabs/themis/pull/578)).
  - Fixed various edge-case correctness issues pointed out by sanitizers,
    clang-tidy, and compiler warnings
    ([#540](https://github.com/cossacklabs/themis/pull/540),
     [#545](https://github.com/cossacklabs/themis/pull/545),
     [#554](https://github.com/cossacklabs/themis/pull/554),
     [#570](https://github.com/cossacklabs/themis/pull/570),
     [#597](https://github.com/cossacklabs/themis/pull/597),
     [#613](https://github.com/cossacklabs/themis/pull/613)).
  - Improved memory wiping,
    making sure that sensitive data doesn't stay in memory longer than absolutely necessary
    ([#584](https://github.com/cossacklabs/themis/pull/584),
     [#585](https://github.com/cossacklabs/themis/pull/585),
     [#586](https://github.com/cossacklabs/themis/pull/586),
     [#612](https://github.com/cossacklabs/themis/pull/612)).

  - **Soter** (low-level security core used by Themis)

    - New function `soter_pbkdf2_sha256()` can be used to derive encryption keys from passphrases with PBKDF2 algorithm ([#574](https://github.com/cossacklabs/themis/pull/574)).

  - **Key generation**

    - New function `themis_gen_sym_key()` can be used to securely generate symmetric keys for Secure Cell ([#560](https://github.com/cossacklabs/themis/pull/560)).

  - **Secure Cell**

    - New functions:

      - `themis_secure_cell_encrypt_seal_with_passphrase()`
      - `themis_secure_cell_decrypt_seal_with_passphrase()`

      provide Seal mode API that is safe to use with passphrases
      ([#577](https://github.com/cossacklabs/themis/pull/577),
       [#582](https://github.com/cossacklabs/themis/pull/582),
       [#640](https://github.com/cossacklabs/themis/pull/640)).

  - **Secure Session**

    - Fixed serialization issue in `secure_session_save()` and `secure_session_load()` methods
      ([#658](https://github.com/cossacklabs/themis/pull/658)).

  - **Breaking changes**

    - <a id="0.13.0-drop-0.9.6-compat">Secure Cell compatibility with Themis 0.9.6 is now disabled by default ([#614](https://github.com/cossacklabs/themis/pull/614)).

      Old versions of Themis have been calculating encrypted data length incorrectly, which made Secure Cells encrypted on 64-bit machines impossible to decrypt on 32-bit machines (see [#279](https://github.com/cossacklabs/themis/pull/279) for details).

      Themis 0.10 and later versions include a fix for that issue and a compatiblity workaround that allows to decrypt data encrypted by Themis 0.9.6 on 64-bit platforms. This workaround was enabled by default and could be disabled by setting the `NO_SCELL_COMPAT` varible.

      Since Themis 0.13 the workaround for Themis 0.9.6 compatibility is *disabled* by default (as it has performance implications). It can be enabled if needed by compling with `WITH_SCELL_COMPAT`.

      We are planning to **remove** the workaround completely after Themis 0.9.6 reaches end-of-life in December 2020. Please use this time to migrate existing data if you have been using Themis 0.9.6. To migrate the data, decrypt it and encrypt it back with the latest Themis version.

    - <a id="0.13.0-big-endian">Themis is known to be broken on big-endian architectures</a> ([#623](https://github.com/cossacklabs/themis/pull/623), [#592](https://github.com/cossacklabs/themis/pull/592)).

      Themis has never committed to supporting machines with big-endian architectures.
      However, it was expected to accidentally work to some degree on such machines,
      with certain compatibility restrictions on interaction with little-endian machines.

      Recent changes in Themis Core are known to introduce compatibility issues on big-endian architectures.
      If you believe you are affected by this change, please reach out to us via
      [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

- **Android**

  See also: [Java API updates](#0.13.0-java).

  - Kotlin is now officially supported language on Android
    ([#637](https://github.com/cossacklabs/themis/pull/637)).
  - Fixed a crash when decrypting corrupted Secure Cell data
    ([#639](https://github.com/cossacklabs/themis/pull/639)).
  - Updated embedded BoringSSL to the latest version
    ([#643](https://github.com/cossacklabs/themis/pull/643)).
  - Fixed broken `SecureSession#save` and `SecureSession#restore` methods
    ([#658](https://github.com/cossacklabs/themis/pull/658)).

  - **Breaking changes**

    - Android build now uses Gradle 5.6 and requires Java 8 ([#633](https://github.com/cossacklabs/themis/pull/633)).

      It is no longer possible to build AndroidThemis with Java 7.
      Please upgrade to Java 8 or later version.

  - **Deprecations**

    - Unqualified Gradle targets are now deprecated ([#633](https://github.com/cossacklabs/themis/pull/633)).

      To build Themis for Android, run

          ./gradlew :android:assembleRelease

      instead of

          ./gradlew assembleRelease

      The unqualified form still works for now, but may break in future releases.

- **C++**

  - Secure Cell API updates ([#588](https://github.com/cossacklabs/themis/pull/588))

    - ThemisPP now supports _passphrase API_ of Secure Cell in Seal mode:

      ```c++
      #include <themispp/secure_cell.hpp>

      auto cell = themispp::secure_cell_seal_with_passphrase("string");

      uint8_t[] plaintext = "message";

      std::vector<uint8_t> encrypted = cell.encrypt(plaintext);
      std::vector<uint8_t> decrypted = cell.decrypt(encrypted);
      ```

      You can safely and securely use short, human-readable passphrases as strings with this new API.

      Existing master key API (`themispp::secure_cell_seal` and other modes) should not be used with passphrases or passwords.
      Use master key API with symmetric encryption keys, such as generated by `themispp::gen_sym_key()` ([#561](https://github.com/cossacklabs/themis/pull/561)).
      Use passphrase API with human-readable passphrases.

    - All modes of Secure Cell get a new initialisation API to avoid ambiguity over whether Secure Cell is secured with a passphrase or a master key (since both are effectively byte arrays in C++):

      - `themispp::secure_cell_seal_with_key(master_key)`
      - `themispp::secure_cell_token_protect_with_key(master_key)`
      - `themispp::secure_cell_context_imprint_with_key(master_key)`

      New API has additional benefits:

      - broader range of input types is accepted, including STL-compatible containers such as `std::vector`, `std::array`, `std::span`, C arrays, etc.
      - Token Protect API is much easier to use
      - Secure Cell is now thread-safe

    - **Deprecated API**

      The following classes are deprecated:

      - `themispp::secure_cell_seal_t`
      - `themispp::secure_cell_token_protect_t`
      - `themispp::secure_cell_context_imprint_t`

      They should be replaced with their `_with_key` counterparts. In most cases migration should be a trivial renaming but there are caveats with Token Protect mode and iterator usage. Please see [#588](https://github.com/cossacklabs/themis/pull/588) for details.

  - New function `themispp::gen_sym_key()` can be used to generate symmetric keys for Secure Cell
    ([#561](https://github.com/cossacklabs/themis/pull/561),
     [#576](https://github.com/cossacklabs/themis/pull/576)).
  - Updated test suite to test C++14 and C++17 (in addition to C++11 and C++03) ([#572](https://github.com/cossacklabs/themis/pull/572)).

  - **Breaking changes**

    - `get_pub_key_by_id()` method of `secure_session_callback_interface_t`
      now has to return non-const vector
      ([#540](https://github.com/cossacklabs/themis/pull/540)).

      Change your implementation like this:

      ```diff
      -const std::vector<uint8_t> get_pub_key_by_id(const std::vector<uint8_t>& id) override
      +std::vector<uint8_t> get_pub_key_by_id(const std::vector<uint8_t>& id) override
       {
           // ...
       }
      ```

- **Go**

  - New function `keys.NewSymmetricKey()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Improved `ThemisError` introspection: added error constants, numeric error codes ([#622](https://github.com/cossacklabs/themis/pull/622)).

  - Secure Cell API updates:

    - New API with improved usability and consistent naming ([#624](https://github.com/cossacklabs/themis/pull/624)).

      ```go
      func SealWithKey(key *keys.SymmetricKey) (*SecureCellSeal, error)
          func (sc *SecureCellSeal) Encrypt(plaintext, context []byte) ([]byte, error)
          func (sc *SecureCellSeal) Decrypt(encrypted, context []byte) ([]byte, error)

      func TokenProtectWithKey(key *keys.SymmetricKey) (*SecureCellTokenProtect, error)
          func (sc *SecureCellTokenProtect) Encrypt(plaintext, context []byte) (encrypted, token []byte, error)
          func (sc *SecureCellTokenProtect) Decrypt(encrypted, token, context []byte) ([]byte, error)

      func ContextImprintWithKey(key *keys.SymmetricKey) (*SecureCellContextImprint, error)
          func (sc *SecureCellContextImprint) Encrypt(plaintext, context []byte) ([]byte, error)
          func (sc *SecureCellContextImprint) Decrypt(encrypted, context []byte) ([]byte, error)
      ```

      This API is less ambiguous and more convenient to use.

    - GoThemis now supports _passphrase API_ in Seal mode ([#625](https://github.com/cossacklabs/themis/pull/625)).

      ```go
      scell, err := cell.SealWithPassphrase("secret")
      if err != nil {
              return err
      }

      encrypted, err := scell.Encrypt([]byte("message"), nil)
      if err != nil {
              return err
      }

      decrypted, err := scell.Decrypt(encrypted, nil)
      if err != nil {
              return err
      }
      ```

      You can safely and securely use short, human-readable passphrases as strings with this new API.

      Existing master key API (`cell.SealWithKey()` or `cell.New()`) should not be used with passphrases or passwords.
      Use master key API with symmetric encryption keys, such as generated by `keys.NewSymmetricKey()` ([#561](https://github.com/cossacklabs/themis/pull/561)).
      Use passphrase API with human-readable passphrases.

  - **Deprecated API**

    - Run-time mode-setting for Secure Cell is deprecated ([#624](https://github.com/cossacklabs/themis/pull/624)).

      Please use new constructors `cell.SealWithKey()` instead of `cell.New()` and `cell.ModeSeal...` constants.
      Encryption is now performed with `Encrypt()` method instead of `Protect()`.
      For decryption use `Decrypt()` instead of `Unprotect()`.

      Old API is retained for compatibility.

- **iOS and macOS**

  - New function `TSGenerateSymmetricKey()` (available in Objective-C and Swift) can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Mac Catalyst is explicitly disabled
    ([#598](https://github.com/cossacklabs/themis/pull/598)).
  - Improved test coverage of platforms
    ([#599](https://github.com/cossacklabs/themis/pull/599),
     [#607](https://github.com/cossacklabs/themis/pull/607),
     [#610](https://github.com/cossacklabs/themis/pull/610),
     [#642](https://github.com/cossacklabs/themis/pull/642)).
  - SwiftThemis is now tested with Swift 5
    ([#605](https://github.com/cossacklabs/themis/pull/605)).
  - iPadOS is now officially supported target for ObjCThemis
    ([#641](https://github.com/cossacklabs/themis/pull/641)).

  - Secure Cell API updates:

    - New encryption/decryption API with consistent naming: `encrypt` and `decrypt` ([#606](https://github.com/cossacklabs/themis/pull/606)).

    - Improved Token Protect API ([#606](https://github.com/cossacklabs/themis/pull/606)):
      - Encryption results use `NSData` now which bridges with Swift `Data` directly.
      - Decryption no longer requires an intermediate `TSCellTokenEncryptedData` object.

    - ObjCThemis now supports _passphrase API_ of in Seal mode ([#609](https://github.com/cossacklabs/themis/pull/609)).

      In Swift:

      ```swift
      let cell = TSCellSeal(passphrase: "secret")

      let encrypted = try cell.encrypt("message".data(using: .utf8)!)
      let decrypted = try cell.decrypt(encrypted)
      ```

      In Objective-C:

      ```objective-c
      TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase:@"secret"];

      NSData *encrypted = [cell encrypt:[@"message" dataUsingEncoding:NSUTF8StringEncoding]];
      NSData *decrypted = [cell decrypt:encrypted];
      ```

      You can safely and securely use short, human-readable passphrases as strings with this new API.

      Existing master key API (`TSCellSeal(key: ...)` or `initWithKey:...`) should not be used with passphrases or passwords.
      Use master key API with symmetric encryption keys, such as generated by `TSGenerateSymmetricKey()` ([#561](https://github.com/cossacklabs/themis/pull/561)).
      Use passphrase API with human-readable passphrases.

  - **Deprecated API**

    - Secure Cell wrapData/unwrapData renamed into encrypt/decrypt ([#606](https://github.com/cossacklabs/themis/pull/606)).

      As a result, the following methods are deprecated. There are no plans for their removal.

      <details>
      <summary>Swift</summary>
      <table>
        <tr><th>Mode</th><th>Deprecation</th><th>Replacement</th></tr>
        <tr>
          <td rowspan=2><code>TSCellSeal</code></td>
          <td><code>wrap(_:, context:)</code><br/><code>wrap</code></td>
          <td><code>encrypt(_:, context:)</code><br/><code>encrypt</code></td>
        </tr>
        <tr>
          <td><code>unwrapData(_:, context:)</code><br/><code>unwrapData</code></td>
          <td><code>decrypt(_:, context:)</code><br/><code>decrypt</code></td>
        </tr>
        <tr>
          <td rowspan=2><code>TSCellToken</code></td>
          <td><code>wrap(_:, context:)</code><br/><code>wrap</code></td>
          <td><code>encrypt(_:, context:)</code><br/><code>encrypt</code></td>
        </tr>
        <tr>
          <td><code>unwrapData(_:, context:)</code><br/><code>unwrapData</code></td>
          <td><code>decrypt(_:, token:, context:)</code><br/><code>decrypt(_:, token:)</code></td>
        </tr>
        <tr>
          <td rowspan=2><code>TSCellContextImprint</code></td>
          <td><code>wrap(_:, context:)</code><br/><code>wrap</code></td>
          <td><code>encrypt(_:, context:)</code><br/><code>encrypt</code></td>
        </tr>
        <tr>
          <td><code>unwrapData(_:, context:)</code><br/><code>unwrapData</code></td>
          <td><code>decrypt(_:, context:)</code><br/><code>decrypt</code></td>
        </tr>
      </table>
      </details>

      <details>
      <summary>Objective-C</summary>
      <table>
        <tr><th>Mode</th><th>Deprecation</th><th>Replacement</th></tr>
        <tr>
          <td rowspan=2><code>TSCellSeal</code></td>
          <td><code>wrapData:context:error:</code><br><code>wrapData:error:</code></td>
          <td><code>encrypt:context:error:</code><br><code>encrypt:error:</code></td>
        </tr>
        <tr>
          <td><code>unwrapData:context:error:</code><br><code>unwrapData:error:</code></td>
          <td><code>decrypt:context:error:</code><br><code>decrypt:error:</code></td>
        </tr>
        <tr>
          <td rowspan=2><code>TSCellToken</code></td>
          <td><code>wrapData:context:error:</code><br><code>wrapData:error:</code></td>
          <td><code>encrypt:context:error:</code><br><code>encrypt:error:</code></td>
        </tr>
        <tr>
          <td><code>unwrapData:context:error:</code><br><code>unwrapData:error:</code></td>
          <td><code>decrypt:token:context:error:</code><br><code>decrypt:token:error:</code></td>
        </tr>
        <tr>
          <td rowspan=2><code>TSCellContextImprint</code></td>
          <td><code>wrapData:context:error:</code><br><code>wrapData:error:</code></td>
          <td><code>encrypt:context:error:</code><br><code>encrypt:error:</code></td>
        </tr>
        <tr>
          <td><code>unwrapData:context:error:</code><br><code>unwrapData:error:</code></td>
          <td><code>decrypt:context:error:</code><br><code>decrypt:error:</code></td>
        </tr>
      </table>
      </details>

  - **Breaking changes**

    - <a id="0.13.0-objcthemis-rename">ObjCThemis framework built by Carthage is now called `objcthemis.framework`</a> ([#604](https://github.com/cossacklabs/themis/pull/604)).

      We have renamed the Carthage framework from `themis.framework` to `objcthemis.framework` in order to improve compatibility with CocoaPods and avoid possible import conflicts with Themis Core.

      > ⚠️ Please migrate to `objcthemis.framework` in a timely manner. `themis.framework` is *deprecated* since Themis 0.13 and will be **removed** in the next release due to maintainability issues.
      >
      > ℹ️ Installations via CocoaPods are *not affected*. If you get Themis via CocoaPods then no action is necessary.

      <details>
      <summary>Migration instructions (click to reveal)</summary>

      After upgrading to Themis 0.13 and running `carthage update` you will notice that _two_ Themis projects have been built:

      ```
      *** Building scheme "OpenSSL (iOS)" in OpenSSL.xcodeproj
      *** Building scheme "ObjCThemis (iOS)" in ObjCThemis.xcodeproj
      *** Building scheme "Themis (iOS)" in Themis.xcodeproj
      ```

      Your project is currently using “Themis”. In order to migrate to “ObjCThemis” you need to do the following:

        - update `#import` statements in code (for Objective-C only)

        - link against `objcthemis.framework` in Xcode project
        - remove link to `themis.framework` in Xcode project

      Use the new syntax to import ObjCThemis in Objective-C projects:

      ```objective-c
      // NEW:
      #import <objcthemis/objcthemis.h>

      // old and deprecated:
      #import <themis/themis.h>
      ```

      The new syntax is now the same as used by CocoaPods.

      If you are using Swift, the import syntax is unchanged:

      ```swift
      import themis
      ```

      After updating imports you *also* need to link against the new framework (regardless of the language).

      1. Add `objcthemis.framework` to your project (can be found in `Carthage/Build/iOS` or `Mac`).
      2. For each Xcode target:

         1. Open **General** tab, **Frameworks and Libraries** section
         2. Drag `objcthemis.framework` there. Select _Embed & Sign_ if necessary.
         3. Remove `themis.framework` from dependencies.

      3. Finally, remove `themis.framework` reference from the project.

      Migration is complete, your project should build successfully now.

      We are sorry for the inconvenience.

      </details>

- <a id="0.13.0-java">**Java**</a>

  - JDK location is now detected automatically in most cases, you should not need to set JAVA_HOME or JDK_INCLUDE_PATH manually ([#551](https://github.com/cossacklabs/themis/pull/551)).
  - JNI libraries are now available as `libthemis-jni` packages for supported Linux systems ([#552](https://github.com/cossacklabs/themis/pull/552), [#553](https://github.com/cossacklabs/themis/pull/553)).
  - Fixed a NullPointerException bug in `SecureSocket` initialisation ([#557](https://github.com/cossacklabs/themis/pull/557)).
  - Some Themis exceptions have been converted from checked `Exception` to _unchecked_ `RuntimeException`, relaxing requirements for `throws` specifiers ([#563](https://github.com/cossacklabs/themis/pull/563)).
  - Introduced `IKey` interface with accessors to raw key data ([#564](https://github.com/cossacklabs/themis/pull/564)).
  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#565](https://github.com/cossacklabs/themis/pull/565)).
  - It is now possible to build desktop Java with Gradle.
    Run `./gradlew :desktop:tasks` to learn more
    ([#633](https://github.com/cossacklabs/themis/pull/633)).
  - Kotlin is now officially supported language for JavaThemis
    ([#637](https://github.com/cossacklabs/themis/pull/637)).
  - Fixed broken `SecureSession#save` and `SecureSession#restore` methods
    ([#658](https://github.com/cossacklabs/themis/pull/658)).
  - Java source code is now ASCII-only for improved compatibility
    ([#655](https://github.com/cossacklabs/themis/pull/655)).

  - Secure Cell API updates:

    - New encryption/decryption API with consistent naming: `encrypt` and `decrypt`
      ([#634](https://github.com/cossacklabs/themis/pull/634)).
    - Improved Token Protect API
      ([#634](https://github.com/cossacklabs/themis/pull/634)).
      - Decryption no longer requires an intermediate `SecureCellData` object.
      - `SecureCellData` can now be destructured in Kotlin
        ([#638](https://github.com/cossacklabs/themis/pull/638)).

        ```kotlin
        // You can now write like this:
        val (encrypted, authToken) = cellTP.encrypt(message, context)

        // Instead of having to spell it out like this:
        val result = cellTP.protect(context, message)
        val encrypted = result.protectedData
        val authToken = result.additionalData
        ```

    - Secure Cell mode can now be selected by instantiating an appropriate interface:

      | New API | Old API |
      | ------- | ------- |
      | `SecureCell.SealWithKey(key)`                 | `new SecureCell(key, SecureCell.MODE_SEAL)` |
      | `SecureCell.SealWithPassphrase(passphrase)`   | _not available_ |
      | `SecureCell.TokenProtectWithKey(key)`         | `new SecureCell(key, SecureCell.MODE_TOKEN_PROTECT)` |
      | `SecureCell.ContextImprintWithKey(key)`       | `new SecureCell(key, SecureCell.MODE_CONTEXT_IMPRINT)` |

    - JavaThemis now supports _passphrase API_ of in Seal mode
      ([#635](https://github.com/cossacklabs/themis/pull/635)).

      In Kotlin:

      ```kotlin
      import com.cossacklabs.themis.SecureCell

      val cell = SecureCell.SealWithPassphrase("secret")

      val message = "message".toByteArray()

      val encrypted = cell.encrypt(message)
      val decrypted = cell.decrypt(encrypted)

      assertArrayEquals(decrypted, message)
      ```

      In Java:

      ```java
      import com.cossacklabs.themis.SecureCell;

      SecureCell.Seal cell = SecureCell.SealWithPassphrase("secret");

      byte[] message = "message".getBytes(StandardCharsets.UTF_8);

      byte[] encrypted = cell.encrypt(message);
      byte[] decrypted = cell.decrypt(encrypted);

      assertArrayEquals(decrypted, message);
      ```

      You can safely and securely use short, human-readable passphrases as strings with this new API.

      Existing symmetric key API (`SecureCell.SealWithKey(...)` or `new SecureCell(...)`)
      should not be used with passphrases or passwords.
      Use symmetric key API with symmetric encryption keys,
      such as generated by `SymmetricKey` ([#565](https://github.com/cossacklabs/themis/pull/565)).
      Use passphrase API with human-readable passphrases.

  - **Deprecated API**

    - Secure Cell has received API overhaul which deprecates old API
      ([#636](https://github.com/cossacklabs/themis/pull/636)).

      The following items are deprecated:

        - Constructors:
          - `new SecureCell(int mode)`
          - `new SecureCell(byte[] key)`
          - `new SecureCell(byte[] key, int mode)`
          - `new SecureCell(String password)` ⚠️ **not recommended, insecure**
          - `new SecureCell(String password, int mode)` ⚠️ **not recommended, insecure**
        - Methods:
          - `protect(byte[] key, byte[] context, byte[] data)`
          - `protect(byte[] constext, byte[] data)`
          - `protect(String password, String context, byte[] data)` ⚠️ **not recommended, insecure**
          - `protect(String context, byte[] data)`
          - `unprotect(byte[] key, byte[] context, SecureCellData protected)`
          - `unprotect(byte[] context, SecureCellData protected)`
          - `unprotect(String password, String context, SecureCellData protected)` ⚠️ **not recommended, insecure**
          - `unprotect(String context, SecureCellData protected)`
        - Constants:
          - `SecureCell.MODE_SEAL`
          - `SecureCell.MODE_TOKEN_PROTECT`
          - `SecureCell.MODE_CONTEXT_IMPRINT`

      Some methods are not secure when used with short passphrases,
      consider using new passphrase API instead.
      Other methods have easier to use replacements in the new API,
      consider using them instead.

      Deprecated API is still supported, there are no plans for its removal.

    - <a id="0.13.0-deprecate-session-save-restore"></a>
      `SecureSession` methods `save` and `restore` are now deprecated
      ([#659](https://github.com/cossacklabs/themis/pull/659)).

      An improved API for serialisation might appear in some next version of JavaThemis.
      For now, please refrain from using `SecureSession#save` and `SecureSession#restore`
      which may be removed in the future.

- **Node.js**

  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#562](https://github.com/cossacklabs/themis/pull/562)).
  - New makefile target `make jsthemis` can be used to build JsThemis from source ([#618](https://github.com/cossacklabs/themis/pull/618)).
  - `SecureCell` now allows `null` to explicitly specify omitted encryption context ([#620](https://github.com/cossacklabs/themis/pull/620)).
  - `SecureMessage` now allows `null` for omitted keys in sign/verify mode ([#620](https://github.com/cossacklabs/themis/pull/620)).
  - Fixed a crash when an exception is thrown from `SecureSession` callback ([#620](https://github.com/cossacklabs/themis/pull/620)).
  - Node.js v14 is now supported
    ([#654](https://github.com/cossacklabs/themis/pull/654)).

  - Passphrase API support in Secure Cell ([#621](https://github.com/cossacklabs/themis/pull/621)).

    JsThemis now supports _passphrase API_ of Secure Cell in Seal mode:

    ```javascript
    const themis = require('jsthemis')

    let cell = themis.SecureCellSeal.withPassphrase('secret')

    let encrypted = cell.encrypt(Buffer.from('message data'))
    let decrypted = cell.decrypt(encrypted)
    ```

    You can safely and securely use short, human-readable passphrases as strings with this new API.

    Existing master key API (available as `themis.SecureCellSeal.withKey(...)`) should not be used with passphrases or passwords.
    Use master key API with symmetric encryption keys, such as generated by `SymmetricKey` ([#562](https://github.com/cossacklabs/themis/pull/562)).
    Use passphrase API with human-readable passphrases.

  - **Deprecated API**

    - Secure Cell construction with `new` is deprecated ([#621](https://github.com/cossacklabs/themis/pull/621)).

      Passphrase API makes it ambiguous whether a Secure Cell is initialised with a master key or a passphrase.
      All Secure Cell classes – `SecureCellSeal`, `SecureCellTokenProtect`, `SecureCellContextImprint` –
      get a static factory method `withKey` to reduce the ambiguity.
      Please use it instead:

      ```javascript
      // NEW, write like this:
      let cell = themis.SecureCellSeal.withKey(secret)

      // old, avoid this:
      let cell = new themis.SecureCellSeal(secret)
      ```

      `new` constructors are not recommended for use but they are still supported and will always work with master keys, as they did before.

- **PHP**

  - New function `phpthemis_gen_sym_key()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Resolved PHP Composer checksum issues once and for all
    ([#566](https://github.com/cossacklabs/themis/pull/566),
     [#567](https://github.com/cossacklabs/themis/pull/567)).
  - PHPThemis now supports _passphrase API_ of Secure Cell in Seal mode ([#594](https://github.com/cossacklabs/themis/pull/594), [#601](https://github.com/cossacklabs/themis/pull/601)).

    ```php
    $encrypted = phpthemis_scell_seal_encrypt_with_passphrase('passphrase', 'message');
    $decrypted = phpthemis_scell_seal_decrypt_with_passphrase('passphrase', $encrypted);
    ```

    You can safely and securely use short, human-readable passphrases as strings with this new API.

    Existing master key API (`phpthemis_scell_seal_{encrypt,decrypt}` and other modes) should not be used with passphrases or passwords.
    Use master key API with symmetric encryption keys, such as generated by `phpthemis_gen_sym_key()` ([#561](https://github.com/cossacklabs/themis/pull/561)).
    Use passphrase API with human-readable passphrases.

- **Python**

  - Fixed compatibility issues on 32-bit platforms ([#555](https://github.com/cossacklabs/themis/pull/555)).
  - New function `skeygen.GenerateSymmetricKey()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - PyThemis now supports _passphrase API_ of Secure Cell in Seal mode ([#596](https://github.com/cossacklabs/themis/pull/596)).

    ```python
    from pythemis.scell import SCellSeal

    cell = SCellSeal(passphrase='my passphrase')

    encrypted = cell.encrypt(b'message data')
    decrypted = cell.decrypt(encrypted)
    ```

    You can safely and securely use short, human-readable passphrases as strings with this new API.

    Existing master key API (`SCellSeal(key=...)`) should not be used with passphrases or passwords.
    Use master key API with symmetric encryption keys, such as generated by `GenerateSymmetricKey()` ([#561](https://github.com/cossacklabs/themis/pull/561)).
    Use passphrase API with human-readable passphrases.
  - <a id="0.13.0-drop-python-2">Python 2 is no longer supported</a>
    ([#648](https://github.com/cossacklabs/themis/pull/648)).

    Python 2 had reached EOL on 2020-01-01.

    In fact, we are not making any changes in this release that break compatibility, but we no longer officially support it. This means that we do not run any CI tests for Python 2, and in the future we will develop code compatible only with Python 3+.

- **Ruby**

  - New function `Themis::gen_sym_key()` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Secure Cell API updates ([#603](https://github.com/cossacklabs/themis/pull/603)).

    - RbThemis now supports _passphrase API_ of Secure Cell in Seal mode:

      ```ruby
      require 'rbthemis'

      cell = Themis::ScellSealPassphrase.new('secret string')

      encrypted = cell.encrypt('message data')
      decrypted = cell.decrypt(encrypted)
      ```

      You can safely and securely use short, human-readable passphrases as strings with this new API.

      Existing master key API (`Themis::Scell...`) should not be used with passphrases or passwords.
      Use master key API with symmetric encryption keys, such as generated by `Themis::gen_sym_key` ([#561](https://github.com/cossacklabs/themis/pull/561)).
      Use passphrase API with human-readable passphrases.

    - Secure Cell mode can now be selected by instantiating an appropriate subclass:

      | New API | Old API |
      | ------- | ------- |
      | `Themis::ScellSeal.new(key)`                  | `Themis::Scell.new(key, Themis::Scell::SEAL_MODE)`            |
      | `Themis::ScellSealPassphrase.new(passphrase)` | _not available_                                               |
      | `Themis::ScellTokenProtect.new(key)`          | `Themis::Scell.new(key, Themis::Scell::TOKEN_PROTECT_MODE)`   |
      | `Themis::ScellContextImprint.new(key`         | `Themis::Scell.new(key, Themis::Scell::CONTEXT_IMPRINT_MODE)` |

      `Themis::Scell` class is **deprecated** and should be replaced with new API.

    - Token Protect mode now accepts encrypted data and token as separate arguments instead of requiring an array:

      ```ruby
      decrypted = cell.decrypt([encrypted, token], context) # old
      decrypted = cell.decrypt(encrypted, token, context)   # new
      ```

      (Arrays are still accepted for compatibility but this API is deprecated.)

- **Rust**

  - New object `themis::keys::SymmetricKey` can be used to generate symmetric keys for Secure Cell
    ([#561](https://github.com/cossacklabs/themis/pull/561),
     [#631](https://github.com/cossacklabs/themis/pull/631)).
  - Significantly reduced compilation time by removing `bindgen` crate from dependencies ([#626](https://github.com/cossacklabs/themis/pull/626)).
  - Bindgen 0.54.1 or later is now required for RustThemis development
    ([#664](https://github.com/cossacklabs/themis/pull/664)).
  - Passphrase API support in Secure Cell ([#630](https://github.com/cossacklabs/themis/pull/630)).

    RustThemis now supports _passphrase API_ of Secure Cell in Seal mode:

    ```rust
    use themis::secure_cell::SecureCell;

    let cell = SecureCell::with_passphase("secret")?.seal();

    let encrypted = cell.encrypt(b"message data")?;
    let decrypted = cell.decrypt(&encrypted)?;
    ```

    You can safely and securely use short, human-readable passphrases as strings with this new API.

    Existing master key API (available as `SecureCell::with_key(...)`) should not be used with passphrases or passwords.
    Use master key API with symmetric encryption keys, such as generated by `themis::keys::SymmetricKey` ([#561](https://github.com/cossacklabs/themis/pull/561)).
    Use passphrase API with human-readable passphrases.

  - Miscellaneous minor improvements in code quality
    ([#571](https://github.com/cossacklabs/themis/pull/571),
     [#591](https://github.com/cossacklabs/themis/pull/591)).

- **WebAssembly**

  - New class `SymmetricKey` can be used to generate symmetric keys for Secure Cell ([#561](https://github.com/cossacklabs/themis/pull/561)).
  - Fixed an issue with webpack Terser plugin
    ([#568](https://github.com/cossacklabs/themis/pull/568)).
  - Updated Emscripten toolchain to the latest version
    ([#550](https://github.com/cossacklabs/themis/pull/550),
     [#569](https://github.com/cossacklabs/themis/pull/569),
     [#602](https://github.com/cossacklabs/themis/pull/602),
     [#653](https://github.com/cossacklabs/themis/pull/653)).
  - Updated embedded BoringSSL and other dependencies to the latest versions
    ([#608](https://github.com/cossacklabs/themis/pull/608),
     [#643](https://github.com/cossacklabs/themis/pull/643)).
  - Node.js v14 is now supported
    ([#654](https://github.com/cossacklabs/themis/pull/654)).

  - Passphrase API support in Secure Cell ([#616](https://github.com/cossacklabs/themis/pull/616)).

    WasmThemis now supports _passphrase API_ of Secure Cell in Seal mode:

    ```javascript
    const themis = require('wasm-themis')

    let cell = themis.SecureCellSeal.withPassphrase('secret')

    let encrypted = cell.encrypt(Buffer.from('message data'))
    let decrypted = cell.decrypt(encrypted)
    ```

    You can safely and securely use short, human-readable passphrases as strings with this new API.

    Existing master key API (available as `themis.SecureCellSeal.withKey(...)`) should not be used with passphrases or passwords.
    Use master key API with symmetric encryption keys, such as generated by `SymmetricKey` ([#561](https://github.com/cossacklabs/themis/pull/561)).
    Use passphrase API with human-readable passphrases.

  - **Deprecated API**

    - Secure Cell construction with `new` is deprecated ([#616](https://github.com/cossacklabs/themis/pull/616)).

      Passphrase API makes it ambiguous whether a Secure Cell is initialised with a master key or a passphrase. All Secure Cell classes –
      `SecureCellSeal`, `SecureCellTokenProtect`, `SecureCellContextImprint` – get a static factory method `withKey` to reduce the ambiguity. Please use it instead:

      ```javascript
      // NEW, write like this:
      let cell = themis.SecureCellSeal.withKey(secret)

      // old, avoid this:
      let cell = new themis.SecureCellSeal(secret)
      ```

      `new` constructors are not recommended for use but they are still supported and will always work with master keys, as they did before.

_Docs:_

- New improved design and structure of [Themis documentation](https://docs.cossacklabs.com/products/themis/).
- Updated templates for GitHub issues and pull requests
  ([#549](https://github.com/cossacklabs/themis/pull/549)).
- Miscellaneous quality improvements in various pieces of documentation
  ([#558](https://github.com/cossacklabs/themis/pull/558),
   [#575](https://github.com/cossacklabs/themis/pull/575),
   [#581](https://github.com/cossacklabs/themis/pull/581),
   [#587](https://github.com/cossacklabs/themis/pull/587),
   [#590](https://github.com/cossacklabs/themis/pull/590)).
- Clarified information on data privacy regulations
  ([#593](https://github.com/cossacklabs/themis/pull/593)).
- Removed last surviving links to deprecated GitHub Wiki
  ([#589](https://github.com/cossacklabs/themis/pull/589)).

_Infrastructure:_

- Changed name of the tarball produced by `make dist` to `themis_X.Y.Z.tar.gz`
  ([#544](https://github.com/cossacklabs/themis/pull/544)).
- Fixed Doxygen support
  ([#559](https://github.com/cossacklabs/themis/pull/559)).
- Automated benchmarking harness is now tracking Themis performance.
  See [`benches`](https://github.com/cossacklabs/themis/tree/master/benches/)
  ([#580](https://github.com/cossacklabs/themis/pull/580),
   [#582](https://github.com/cossacklabs/themis/pull/582)).
- Automated regular fuzzing of the code with AFL
  ([#579](https://github.com/cossacklabs/themis/pull/579),
   [#583](https://github.com/cossacklabs/themis/pull/583)).
- Added automated tests for all code samples in documentation, ensuring they are always up-to-date ([#600](https://github.com/cossacklabs/themis/pull/600)).
- All 13 supported platforms are verified on GitHub Actions, along with existing CircleCI and Bitrise tests ([#600](https://github.com/cossacklabs/themis/pull/600)).
- New Makefile targets:
  - `make jsthemis` builds JsThemis from source ([#618](https://github.com/cossacklabs/themis/pull/618)).
- Resolved issues with library search paths on CentOS
  when Themis Core is built from source and installed with `make install`
  ([#645](https://github.com/cossacklabs/themis/pull/645).
- Resolved issues with library search paths on Debian
  when Themis Core is installed from packages
  ([#651](https://github.com/cossacklabs/themis/pull/651)).
- Introduced `./configure` script to significantly improve rebuild performance
  ([#611](https://github.com/cossacklabs/themis/pull/611),
   [#628](https://github.com/cossacklabs/themis/pull/628)).
- Improved package installation testing and platform coverage
  ([#595](https://github.com/cossacklabs/themis/pull/595),
   [#650](https://github.com/cossacklabs/themis/pull/650)).
- Miscellaneous minor improvements and updates in the build system
  ([#542](https://github.com/cossacklabs/themis/pull/542),
   [#573](https://github.com/cossacklabs/themis/pull/573),
   [#615](https://github.com/cossacklabs/themis/pull/615),
   [#617](https://github.com/cossacklabs/themis/pull/617),
   [#629](https://github.com/cossacklabs/themis/pull/629),
   [#627](https://github.com/cossacklabs/themis/pull/627),
   [#632](https://github.com/cossacklabs/themis/pull/632),
   [#644](https://github.com/cossacklabs/themis/pull/644),
   [#646](https://github.com/cossacklabs/themis/pull/646),
   [#649](https://github.com/cossacklabs/themis/pull/649),
   [#656](https://github.com/cossacklabs/themis/pull/656)).

- <a id="0.13.0-new-platforms">**New supported platforms**</a>

  - CentOS 8 is now fully fully supported.
  - Ubuntu 20.04 “Focal Fossa” is now fully fully supported.
  - GoThemis is now tested with Go 1.14
    ([#595](https://github.com/cossacklabs/themis/pull/595)).
  - SwiftThemis is now tested with Swift 5
    ([#605](https://github.com/cossacklabs/themis/pull/605)).
  - Kotlin API of JavaThemis is now verified by all CI platforms
    ([#637](https://github.com/cossacklabs/themis/pull/637)).
  - iPadOS is now officially supported target for ObjCThemis
    ([#641](https://github.com/cossacklabs/themis/pull/641)).
  - Node.js v14 is now supported for JsThemis and WasmThemis
    ([#654](https://github.com/cossacklabs/themis/pull/654)).

- **Breaking changes**

  - <a id="0.13.0-drop-java-7">Java 7 is no longer supported</a>
    ([#633](https://github.com/cossacklabs/themis/pull/633)).

    Updates in Gradle build infrastructure require Java 8.

  - Debian 8 “Jessie” is no longer supported
    ([#633](https://github.com/cossacklabs/themis/pull/633)).

    This version is no longer maintained by the Debian team and it lacks Java 8.
    We no longer provide binary packages for this distribution.

  - Python 2 is no longer supported
    ([#648](https://github.com/cossacklabs/themis/pull/648)).

    Python 2 had finally reached EOL on 2020-01-01.
    PyThemis 0.13 is the last version guaranteed to be compatible with Python 2.

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
