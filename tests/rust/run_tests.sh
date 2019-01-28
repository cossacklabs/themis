#!/bin/sh

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
cargo test --all

echo
echo "Checking documentation..."
echo
cargo clean --doc && cargo doc --no-deps
cargo clean --doc && cargo doc --no-deps --features "vendored"
cargo deadlinks

echo
echo "Rust tests OK!"
echo
