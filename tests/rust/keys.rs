// Copyright 2018 (c) rust-themis developers
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

use themis::keygen::{gen_ec_key_pair, gen_rsa_key_pair};
use themis::keys::{
    EcdsaPublicKey, EcdsaSecretKey, KeyKind, KeyPair, PublicKey, RsaPublicKey, RsaSecretKey,
    SecretKey,
};
use themis::ErrorKind;

#[test]
fn generated_key_kinds() {
    let (secret_ec, public_ec) = KeyPair::from(gen_ec_key_pair()).split();
    let (secret_rsa, public_rsa) = KeyPair::from(gen_rsa_key_pair()).split();

    assert_eq!(secret_ec.kind(), KeyKind::EcdsaSecret);
    assert_eq!(public_ec.kind(), KeyKind::EcdsaPublic);
    assert_eq!(secret_rsa.kind(), KeyKind::RsaSecret);
    assert_eq!(public_rsa.kind(), KeyKind::RsaPublic);
}

// No cheating here: keys borrowed from PyThemis.
const ECDSA_SECRET: &[u8] = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01";
const ECDSA_PUBLIC: &[u8] = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21";

#[test]
fn parse_bytes() {
    EcdsaSecretKey::try_from_slice(ECDSA_SECRET).expect("ECDSA secret key");
    EcdsaPublicKey::try_from_slice(ECDSA_PUBLIC).expect("ECDSA public key");

    SecretKey::try_from_slice(ECDSA_SECRET).expect("ECDSA secret key (generic)");
    PublicKey::try_from_slice(ECDSA_PUBLIC).expect("ECDSA public key (generic)");

    RsaSecretKey::try_from_slice(ECDSA_SECRET).expect_err("ECDSA secret key (as RSA)");
    RsaPublicKey::try_from_slice(ECDSA_PUBLIC).expect_err("ECDSA public key (as RSA)");

    EcdsaPublicKey::try_from_slice(ECDSA_SECRET).expect_err("ECDSA secret key (as public)");
    EcdsaSecretKey::try_from_slice(ECDSA_PUBLIC).expect_err("ECDSA public key (as secret)");
}

#[test]
fn parse_generated_keys_back() {
    let (secret_gen, public_gen) = gen_rsa_key_pair().split();

    let secret_parse = RsaSecretKey::try_from_slice(&secret_gen).expect("RSA secret key");
    let public_parse = RsaPublicKey::try_from_slice(&public_gen).expect("RSA public key");

    assert_eq!(secret_gen, secret_parse);
    assert_eq!(public_gen, public_parse);
}

#[test]
fn parse_invalid_buffers() {
    let error = EcdsaPublicKey::try_from_slice(&[1, 2, 3]).expect_err("parse failure");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);

    let error = RsaSecretKey::try_from_slice(&[]).expect_err("parse failure");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

#[test]
fn join_matching_keys() {
    let (secret_ec, public_ec) = gen_ec_key_pair().split();

    assert!(KeyPair::try_join(secret_ec, public_ec).is_ok());
}

#[test]
fn join_mismatching_keys() {
    let (secret_ec, _) = gen_ec_key_pair().split();
    let (_, public_rsa) = gen_rsa_key_pair().split();

    let error = KeyPair::try_join(secret_ec, public_rsa).expect_err("kind mismatch");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}
