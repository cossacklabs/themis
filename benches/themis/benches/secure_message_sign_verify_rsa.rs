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

use libthemis_sys::{
    themis_secure_message_sign, themis_secure_message_verify, THEMIS_BUFFER_TOO_SMALL,
    THEMIS_SUCCESS,
};
use themis::{keygen::gen_rsa_key_pair, secure_message::SecureSign};

const KB: usize = 1024;
const MB: usize = 1024 * KB;
#[allow(clippy::identity_op)]
const MESSAGE_SIZES: &[usize] = &[
    16,     // UUID
    32,     // SymmetricKey
    64,     // cache line (and close to EcdsaPrivateKey)
    256,    // RsaPrivateKey
    4 * KB, // memory page
    16 * KB,
    64 * KB,
    1 * MB, // L2 cache
    2 * MB,
    4 * MB,
];

pub fn signing(c: &mut Criterion) {
    let (private, _) = gen_rsa_key_pair().split();
    let private = private.as_ref();

    // Allocate buffer large enough for maximum test
    let mut signature = vec![0; 10 * MB];

    let mut group = c.benchmark_group("Secure Message signing - RSA");
    for message_size in MESSAGE_SIZES {
        group.throughput(Throughput::Bytes(*message_size as u64));
        group.bench_with_input(
            BenchmarkId::from_parameter(pretty(*message_size)),
            message_size,
            |b, &size| {
                let message = vec![0; size];

                b.iter(|| {
                    let mut signature_size = 0;

                    let res = unsafe {
                        themis_secure_message_sign(
                            private.as_ptr(),
                            private.len(),
                            message.as_ptr(),
                            message.len(),
                            std::ptr::null_mut(),
                            &mut signature_size,
                        )
                    };
                    assert_eq!(res, THEMIS_BUFFER_TOO_SMALL as i32);
                    assert!(signature_size <= signature.len());

                    let res = unsafe {
                        themis_secure_message_sign(
                            private.as_ptr(),
                            private.len(),
                            message.as_ptr(),
                            message.len(),
                            signature.as_mut_ptr(),
                            &mut signature_size,
                        )
                    };
                    assert_eq!(res, THEMIS_SUCCESS as i32);
                });
            },
        );
    }
    group.finish();
}

pub fn verification(c: &mut Criterion) {
    let key_pair = gen_rsa_key_pair();
    let (private, public) = key_pair.split();
    let public = public.as_ref();

    let mut group = c.benchmark_group("Secure Message verification - RSA");
    for message_size in MESSAGE_SIZES {
        group.throughput(Throughput::Bytes(*message_size as u64));
        group.bench_with_input(
            BenchmarkId::from_parameter(pretty(*message_size)),
            message_size,
            |b, &size| {
                let message = vec![0; size];
                let signature = SecureSign::new(private.clone())
                    .sign(&message)
                    .expect("failed signing");

                let mut received_message = vec![0; size];
                b.iter(|| {
                    let mut received_message_size = 0;
                    let res = unsafe {
                        themis_secure_message_verify(
                            public.as_ptr(),
                            public.len(),
                            signature.as_ptr(),
                            signature.len(),
                            std::ptr::null_mut(),
                            &mut received_message_size,
                        )
                    };
                    assert_eq!(res, THEMIS_BUFFER_TOO_SMALL as i32);
                    assert!(received_message_size == received_message.len());

                    let res = unsafe {
                        themis_secure_message_verify(
                            public.as_ptr(),
                            public.len(),
                            signature.as_ptr(),
                            signature.len(),
                            received_message.as_mut_ptr(),
                            &mut received_message_size,
                        )
                    };
                    assert_eq!(res, THEMIS_SUCCESS as i32);
                });
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
        format!("{}", size)
    }
}

criterion_group!(benches, signing, verification);
criterion_main!(benches);
