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

mod context_imprint {
    use super::*;

    #[test]
    fn happy_path() {
        let cell = SecureCell::with_key_and_context(b"deep secret", b"123").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell.encrypt(&plaintext).unwrap();
        let recovered = cell.decrypt(&ciphertext).unwrap();

        assert_eq!(recovered, plaintext);

        assert_eq!(plaintext.len(), ciphertext.len());
    }

    #[test]
    fn empty_context() {
        let cell = SecureCell::with_key(b"deep secret").context_imprint();

        let plaintext = b"example plaintext";
        let error = cell.encrypt(&plaintext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::InvalidParameter);
    }

    #[test]
    fn invalid_key() {
        let cell1 = SecureCell::with_key_and_context(b"deep secret", b"123").context_imprint();
        let cell2 = SecureCell::with_key_and_context(b"DEEP SECRET", b"123").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell1.encrypt(&plaintext).unwrap();
        let recovered = cell2.decrypt(&ciphertext).unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    fn invalid_context() {
        let cell1 = SecureCell::with_key_and_context(b"deep secret", b"123").context_imprint();
        let cell2 = SecureCell::with_key_and_context(b"deep secret", b"456").context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell1.encrypt(&plaintext).unwrap();
        let recovered = cell2.decrypt(&ciphertext).unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key_and_context(b"deep secret", b"123").context_imprint();

        let plaintext = b"example plaintext";
        let mut ciphertext = cell.encrypt(&plaintext).unwrap();
        ciphertext[10] = 42;
        let recovered = cell.decrypt(&ciphertext).unwrap();

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
        let seal1 = SecureCell::with_key_and_context(b"deep secret", b"ctx1").seal();
        let seal2 = SecureCell::with_key_and_context(b"deep secret", b"ctx2").seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal1.encrypt(&plaintext).unwrap();
        let error = seal2.decrypt(&ciphertext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_data() {
        let seal = SecureCell::with_key(b"deep secret").seal();

        let plaintext = b"example plaintext";
        let mut ciphertext = seal.encrypt(&plaintext).unwrap();
        ciphertext[10] = 42;
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
        let cell1 = SecureCell::with_key_and_context(b"deep secret", b"123").token_protect();
        let cell2 = SecureCell::with_key_and_context(b"deep secret", b"456").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell1.encrypt(plaintext).unwrap();
        let error = cell2.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (mut ciphertext, token) = cell.encrypt(&plaintext).unwrap();
        ciphertext[10] = 42;
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_token() {
        let cell = SecureCell::with_key(b"deep secret").token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, mut token) = cell.encrypt(&plaintext).unwrap();
        token[10] = 42;
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::InvalidParameter);
    }
}
