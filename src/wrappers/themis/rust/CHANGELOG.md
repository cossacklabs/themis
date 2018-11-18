[Unreleased]
============

The version currently under development.

Version 0.0.2 — 2018-11-18
==========================

Improving type safety and dependency management.

## New features

- `SecureComparator` now implements `Default`.

- Type alias `themis::Result` with native Themis error is now available.

- `SecureComparator` now provides `is_complete()` method.

- `SecureCell` and `SecureMessage` now own the provided keys, which will be
  copied if necessary. This relaxes lifetime restrictions on instances
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
