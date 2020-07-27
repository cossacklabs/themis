#!/usr/bin/env sh

set -e

# Treat warnings as errors
export RUSTFLAGS="-D warnings"

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
# Cargo does not allow "--features" to be used with virtual crate in workspace.
# Jump into RustThemis subdirectory to make Cargo happy and use "--workspace"
# to still document all crates in the workspace.
cd src/wrappers/themis/rust
cargo clean --doc && cargo doc --workspace --no-deps
cargo clean --doc && cargo doc --workspace --no-deps --features "vendored"
cd -

echo
echo "Rust tests OK!"
echo
