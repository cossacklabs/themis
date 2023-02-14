// Copyright 2020 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};

use themis::secure_cell::SecureCell;

const CONTEXT: &[u8] = b"Rust Themis benchmark";
const PASSPHRASE: &str = "correct horse battery staple";

const KB: usize = 1024;
const MB: usize = 1024 * KB;
#[allow(clippy::identity_op)]
const MESSAGE_SIZES: &[usize] = &[
    // There is no significant difference between those due to KDF
    64,     // cache line (and close to EcdsaPrivateKey)
    4 * KB, // memory page
    1 * MB, // L2 cache
];

pub fn encryption(c: &mut Criterion) {
    let cell = SecureCell::with_passphrase(PASSPHRASE)
        .expect("invalid key")
        .seal();

    let mut group = c.benchmark_group("RustThemis - Secure Cell encryption - Seal, passphrase");
    for message_size in MESSAGE_SIZES {
        group.throughput(Throughput::Bytes(*message_size as u64));
        group.bench_with_input(
            BenchmarkId::from_parameter(pretty(*message_size)),
            message_size,
            |b, &size| {
                let message = vec![0; size];
                b.iter(|| {
                    let encrypted = cell.encrypt_with_context(&message, CONTEXT);
                    assert!(encrypted.is_ok());
                });
            },
        );
    }
    group.finish();
}

pub fn decryption(c: &mut Criterion) {
    let cell = SecureCell::with_key(PASSPHRASE)
        .expect("invalid key")
        .seal();

    let mut group = c.benchmark_group("RustThemis - Secure Cell decryption - Seal, passphrase");
    for message_size in MESSAGE_SIZES {
        group.throughput(Throughput::Bytes(*message_size as u64));
        group.bench_with_input(
            BenchmarkId::from_parameter(pretty(*message_size)),
            message_size,
            |b, &size| {
                let encrypted = cell
                    .encrypt_with_context(vec![0; size], CONTEXT)
                    .expect("failed encryption");
                b.iter(|| {
                    let decrypted = cell.decrypt_with_context(&encrypted, CONTEXT);
                    assert!(decrypted.is_ok());
                })
            },
        );
    }
    group.finish();
}

fn pretty(size: usize) -> String {
    if size >= MB {
        format!("{} MB", size / MB)
    } else if size >= KB {
        format!("{} KB", size / KB)
    } else {
        format!("{size}")
    }
}

criterion_group!(benches, encryption, decryption);
criterion_main!(benches);
