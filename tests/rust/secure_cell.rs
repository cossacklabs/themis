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

use themis::{secure_cell::SecureCell, ErrorKind};

#[test]
#[should_panic(expected = "empty master key")]
fn empty_master_key() {
    SecureCell::with_key(&[]);
}

mod context_imprint {
    use super::*;

    #[test]
    fn happy_path() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell.decrypt_with_context(&ciphertext, b"123").unwrap();

        assert_eq!(recovered, plaintext);

        assert_eq!(plaintext.len(), ciphertext.len());
    }

    #[test]
    fn invalid_key() {
        let cell1 = SecureCell::with_key(b"deep secret").context_imprint();
        let cell2 = SecureCell::with_key(b"DEEP SECRET").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell1.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell2.decrypt_with_context(&ciphertext, b"123").unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    fn invalid_context() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell.decrypt_with_context(&ciphertext, b"456").unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_encrypt() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let _ = cell.encrypt_with_context(b"", b"non-empty context");
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_decrypt() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let _ = cell.decrypt_with_context(b"", b"non-empty context");
    }

    #[test]
    #[should_panic(expected = "empty user context")]
    fn empty_context_encrypt() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let _ = cell.encrypt_with_context(b"example plaintext", b"");
    }

    #[test]
    #[should_panic(expected = "empty user context")]
    fn empty_context_decrypt() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let _ = cell.decrypt_with_context(b"non-empty message", b"");
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let plaintext = b"example plaintext";
        let mut ciphertext = cell.encrypt_with_context(&plaintext, b"123").unwrap();
        ciphertext[10] = !ciphertext[10];
        let recovered = cell.decrypt_with_context(&ciphertext, b"123").unwrap();

        assert_ne!(recovered, plaintext);
    }
}

mod seal {
    use super::*;

    #[test]
    fn happy_path() {
        let seal = SecureCell::with_key("deep secret").seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal.encrypt(&plaintext).unwrap();
        let recovered = seal.decrypt(&ciphertext).unwrap();

        assert_eq!(recovered, plaintext);
    }

    #[test]
    fn invalid_key() {
        let seal1 = SecureCell::with_key(b"deep secret").seal();
        let seal2 = SecureCell::with_key(b"DEEP SECRET").seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal1.encrypt(&plaintext).unwrap();
        let error = seal2.decrypt(&ciphertext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn invalid_context() {
        let seal = SecureCell::with_key(b"deep secret").seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal.encrypt_with_context(&plaintext, b"ctx1").unwrap();
        let error = seal.decrypt_with_context(&ciphertext, b"ctx2").unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_encrypt() {
        let seal = SecureCell::with_key(b"deep secret").seal();

        let _ = seal.encrypt(b"");
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_decrypt() {
        let seal = SecureCell::with_key(b"deep secret").seal();

        let _ = seal.decrypt(&[]);
    }

    #[test]
    fn corrupted_data() {
        let seal = SecureCell::with_key(b"deep secret").seal();

        let plaintext = b"example plaintext";
        let mut ciphertext = seal.encrypt(&plaintext).unwrap();
        ciphertext[10] = !ciphertext[10];
        let error = seal.decrypt(&ciphertext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::InvalidParameter);
    }
}

mod token_protect {
    use super::*;

    #[test]
    fn happy_path() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell.encrypt(&plaintext).unwrap();
        let recovered = cell.decrypt(&ciphertext, &token).unwrap();

        assert_eq!(recovered, plaintext);

        assert_eq!(plaintext.len(), ciphertext.len());
    }

    #[test]
    fn invalid_key() {
        let cell1 = SecureCell::with_key(b"deep secret").token_protect();
        let cell2 = SecureCell::with_key(b"DEEP SECRET").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell1.encrypt(plaintext).unwrap();
        let error = cell2.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn invalid_context() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell.encrypt_with_context(plaintext, b"123").unwrap();
        let error = cell
            .decrypt_with_context(&ciphertext, &token, b"456")
            .unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_encrypt() {
        let seal = SecureCell::with_key(b"deep secret").token_protect();

        let _ = seal.encrypt(b"");
    }

    #[test]
    #[should_panic(expected = "empty message")]
    fn empty_message_decrypt() {
        let seal = SecureCell::with_key(b"deep secret").token_protect();

        let _ = seal.decrypt(&[], b"non-empty token");
    }

    #[test]
    #[should_panic(expected = "empty token")]
    fn empty_token_decrypt() {
        let seal = SecureCell::with_key(b"deep secret").token_protect();

        let _ = seal.decrypt(b"non-empty message", &[]);
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (mut ciphertext, token) = cell.encrypt(&plaintext).unwrap();
        ciphertext[10] = !ciphertext[10];
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_token() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, mut token) = cell.encrypt(&plaintext).unwrap();
        token[10] = !token[10];
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::InvalidParameter);
    }
}
