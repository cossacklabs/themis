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

#![allow(clippy::needless_borrow)]

use themis::keygen::{gen_ec_key_pair, gen_rsa_key_pair};
use themis::keys::{
    EcdsaPrivateKey, EcdsaPublicKey, KeyKind, KeyPair, PrivateKey, PublicKey, RsaPrivateKey,
    RsaPublicKey, SymmetricKey,
};
use themis::ErrorKind;

#[test]
fn generated_key_kinds() {
    let (private_ec, public_ec) = KeyPair::from(gen_ec_key_pair()).split();
    let (private_rsa, public_rsa) = KeyPair::from(gen_rsa_key_pair()).split();

    assert_eq!(private_ec.kind(), KeyKind::EcdsaPrivate);
    assert_eq!(public_ec.kind(), KeyKind::EcdsaPublic);
    assert_eq!(private_rsa.kind(), KeyKind::RsaPrivate);
    assert_eq!(public_rsa.kind(), KeyKind::RsaPublic);
}

// No cheating here: keys borrowed from PyThemis.
const ECDSA_PRIVATE: &[u8] = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01";
const ECDSA_PUBLIC: &[u8] = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21";

#[test]
fn parse_bytes() {
    EcdsaPrivateKey::try_from_slice(ECDSA_PRIVATE).expect("ECDSA private key");
    EcdsaPublicKey::try_from_slice(ECDSA_PUBLIC).expect("ECDSA public key");

    PrivateKey::try_from_slice(ECDSA_PRIVATE).expect("ECDSA private key (generic)");
    PublicKey::try_from_slice(ECDSA_PUBLIC).expect("ECDSA public key (generic)");

    RsaPrivateKey::try_from_slice(ECDSA_PRIVATE).expect_err("ECDSA private key (as RSA)");
    RsaPublicKey::try_from_slice(ECDSA_PUBLIC).expect_err("ECDSA public key (as RSA)");

    EcdsaPublicKey::try_from_slice(ECDSA_PRIVATE).expect_err("ECDSA private key (as public)");
    EcdsaPrivateKey::try_from_slice(ECDSA_PUBLIC).expect_err("ECDSA public key (as private)");
}

#[test]
fn parse_generated_keys_back() {
    let (private_gen, public_gen) = gen_rsa_key_pair().split();

    let private_parse = RsaPrivateKey::try_from_slice(&private_gen).expect("RSA private key");
    let public_parse = RsaPublicKey::try_from_slice(&public_gen).expect("RSA public key");

    assert_eq!(private_gen, private_parse);
    assert_eq!(public_gen, public_parse);
}

#[test]
fn parse_invalid_buffers() {
    let error = EcdsaPublicKey::try_from_slice(&[1, 2, 3]).expect_err("parse failure");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);

    let error = RsaPrivateKey::try_from_slice(&[]).expect_err("parse failure");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

#[test]
fn join_matching_keys() {
    let (private_ec, public_ec) = gen_ec_key_pair().split();

    assert!(KeyPair::try_join(private_ec, public_ec).is_ok());
}

#[test]
fn join_mismatching_keys() {
    let (private_ec, _) = gen_ec_key_pair().split();
    let (_, public_rsa) = gen_rsa_key_pair().split();

    let error = KeyPair::try_join(private_ec, public_rsa).expect_err("kind mismatch");
    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

#[test]
fn generate_symmetric_keys() {
    let default_size = 32;
    let key = SymmetricKey::new();
    assert_eq!(key.as_ref().len(), default_size);
}

#[test]
fn parse_generated_symmetric_keys_back() {
    let this_key = SymmetricKey::new();
    let same_key = SymmetricKey::try_from_slice(&this_key);
    assert!(same_key.is_ok());
    let same_key = same_key.unwrap();
    assert_eq!(this_key, same_key);
}

#[test]
fn parse_custom_symmetric_keys() {
    assert!(SymmetricKey::try_from_slice(&[0]).is_ok());
    assert!(SymmetricKey::try_from_slice(&[]).is_err());
}
