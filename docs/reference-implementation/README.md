# Reference implementation

Reference implementation of Themis algorithms in Go.

This is a part of [Themis specificiation](https://docs.cossacklabs.com/themis/spec/),
augmenting prose descriptions of core Themis algorithms with machine-testable implementation.

⚠️ **This is not a production-grade implementation.**
If you want to use Themis in Go, please use [GoThemis](https://docs.cossacklabs.com/themis/languages/go/).

The main goal of this reference implementation is clear, correct, and well-documented code,
aiding with understanding of cryptographic algorithms and data formats used in Themis.

Some of the non-goals for the reference implementation:

  - Absolutely no API stability guarantees.

    This code is not intended for consumption by third-party users.
    Expect things to be renamed or restructured with no warning.

  - Completeness does not matter much.

    Reference implementation may not support *all* features supported by GoThemis.
    In particular, various compatibility modes might not be implemented.

  - Performance does not matter.

    Reference implementation makes no effort to use hardware acceleration
    and algorithm implementation might not be well-optimized for execution.
    Readability comes first and will not be compromised for speed.

  - Robustness does not matter.

    Reference implementation might cut some corners with input validation
    and may not be as robust to malformed and malicious input as GoThemis.

    Errors might be signaled with panics if handling them is too bothersome.
