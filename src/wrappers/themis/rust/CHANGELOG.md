[Unreleased]
============

The version currently in development.

Version 0.13.0 - 2020-07-08
===========================

## New features

- New `SymmetricKey` objects can be used as master keys for `SecureCell`.
  ([#561](https://github.com/cossacklabs/themis/pull/561))
- Safe passphrase API of Secure Cell: `SecureCell::with_passphrase`.
  ([#630](https://github.com/cossacklabs/themis/pull/630))

## Internal improvements

- Significantly reduced compilation time by removing `bindgen` crate from dependencies.
  ([#626](https://github.com/cossacklabs/themis/pull/626))

Version 0.12.0 - 2019-09-26
===========================

Multiple updates in Themis Core make the unsafe part of the library
a safer place. Read more [here](https://github.com/cossacklabs/themis/blob/master/CHANGELOG.md#0120-september-26th-2019).

Version 0.11.1 - 2019-04-01
===========================

## Internal improvements

- `libthemis-sys` is now able to use core Themis library installed in
  standard system paths, without _pkg-config_ assistance. ([#444])

[#444]: https://github.com/cossacklabs/themis/pull/444

Version 0.11.0 — 2019-03-28
===========================

The first stable release of Rust-Themis.

## Breaking changes

- `SecureCell` interface has been overhauled for better usability and
  security.

  - User context has to be provided together with messages
    instead of being fixed at construction time. The methods now use
    _impl Trait_ instead of explicit generics as well. ([#358])

  - `SecureCell::with_key()` now checks the master key and returns a
    `Result<SecureCell>` instead of `SecureCell` directly. ([#365])

- `SecureMessage` methods `wrap` and `unwrap` have been renamed into
  `encrypt` and `decrypt` respectively. Their parameters have been
  changed to use _impl Trait_ instead of explicit generics. ([#358])

- Asymmetric key structures have been renamed in order to be consistent
  with other language wrappers. They are now called `PrivateKey` instead
  of `SecretKey` (ditto for `RsaSecretKey`, `EcdsaSecretKey`). ([#362])

- `SecureSession` interface has been overhauled for better usability.
  ([#373], [#374], [#375], [#380])

[#358]: https://github.com/cossacklabs/themis/pull/358
[#362]: https://github.com/cossacklabs/themis/pull/362
[#365]: https://github.com/cossacklabs/themis/pull/365
[#373]: https://github.com/cossacklabs/themis/pull/373
[#374]: https://github.com/cossacklabs/themis/pull/374
[#375]: https://github.com/cossacklabs/themis/pull/375
[#380]: https://github.com/cossacklabs/themis/pull/380

Version 0.0.3 — 2019-01-17
==========================

Documentation and internal improvements.

Themis now requires **Rust 2018** (rustc 1.31+) to compile.

## New features

- Crate feature `vendored` allows to build and use a vendored copy of the core
  Themis library if it is not installed in the system. ([#9])

- All modules, data types, and functions now have proper
  [API documentation][docs.rs], complete with examples and references to
  underlying cryptographic algorithms.

[#9]: https://github.com/ilammy/rust-themis/pull/9
[docs.rs]: https://docs.rs/crate/themis/

Version 0.0.2 — 2018-11-18
==========================

Improving type safety and dependency management.

## New features

- `SecureComparator` now implements `Default`.

- Type alias `themis::Result` with native Themis error is now available.

- `SecureComparator` now provides `is_complete()` method.

- `SecureCell` and `SecureMessage` now own the provided keys, which will be
  copied if necessary. This relaxes the lifetime restrictions on instances
  constructed using byte slices.

- Themis now uses strong types for keys instead of `Vec<u8>` and `&[u8]`.
  See [#4] for details. This is likely a **breaking change**.

- `libthemis-sys` will now use **pkg-config** to locate native Themis library.
  See [#2], [#5], [#7] and README for details.

## Breaking changes

- `SecureSessionState::Negotiate` enumeration variant is now properly spelled
  as `Negotiating` in order to be consistent with the core library.

- `gen_rsa_key_pair()`, `gen_ec_key_pair()`, `SecureComparator::new()` now
  return their results directly instead of wrapping errors into `Result` or
  `Option`. These functions may fail only on likely unrecoverable internal
  errors of Themis so now they simply panic in this case.

- `SecureSession::with_transport()` now returns `Result` instead of `Option`.

[#2]: https://github.com/ilammy/rust-themis/issues/2
[#4]: https://github.com/ilammy/rust-themis/issues/4
[#5]: https://github.com/ilammy/rust-themis/pull/5
[#7]: https://github.com/ilammy/rust-themis/pull/7

Version 0.0.1 — 2018-10-04
==========================

The first release of Themis for Rust.

- Full API coverage:
  * Key generation
  * Secure Cell
  * Secure Message
  * Secure Session
  * Secure Comparator
- Basic API documentation
- Basic test suite
- Basic code samples:
  * Key generation tool
  * File encryption using Secure Cell
  * Relay chat using Secure Message
