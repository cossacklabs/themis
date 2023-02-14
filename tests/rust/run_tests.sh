#!/usr/bin/env sh

set -e

if [ "${WITH_FATAL_WARNINGS:-}" = "yes" ]; then
    # Treat warnings as errors
    export RUSTFLAGS="-D warnings"
fi

echo
echo "Checking code style..."
echo
cargo fmt -- --check

echo "Running static analysis..."
echo
cargo clippy --all --all-targets

echo
echo "Building Themis..."
echo
LIBTHEMIS_DYNAMIC=1 cargo build
LIBTHEMIS_STATIC=1  cargo build

echo
echo "Running tests..."
echo
# Rust tests are multithreaded by default. Themis may be using an old OpenSSL
# which requires global initialization for correct operation. This is hard to
# do in Rust for tests so simply don't use multithreading to avoid failures.
cargo test --all -- --test-threads 1

echo
echo "Checking documentation..."
echo
# Jump into RustThemis subdirectory and use "--workspace" to document
# all crates in the workspace.
cd src/wrappers/themis/rust
cargo clean --doc && cargo doc --workspace --no-deps
cd -

echo
echo "Rust tests OK!"
echo
