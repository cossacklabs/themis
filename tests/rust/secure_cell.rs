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

use themis::{keys::SymmetricKey, secure_cell::SecureCell, ErrorKind};

#[test]
fn empty_master_key() {
    assert!(SecureCell::with_key(b"").is_err());
}

mod context_imprint {
    use super::*;

    #[test]
    fn happy_path() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell.decrypt_with_context(&ciphertext, b"123").unwrap();

        assert_eq!(recovered, plaintext);

        assert_eq!(plaintext.len(), ciphertext.len());
    }

    #[test]
    fn empty_context() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .context_imprint();

        let plaintext = b"example plaintext";
        let error = cell.encrypt_with_context(&plaintext, b"").unwrap_err();

        assert_eq!(error.kind(), ErrorKind::InvalidParameter);
    }

    #[test]
    fn invalid_key() {
        let cell1 = SecureCell::with_key(b"deep secret")
            .unwrap()
            .context_imprint();
        let cell2 = SecureCell::with_key(b"DEEP SECRET")
            .unwrap()
            .context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell1.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell2.decrypt_with_context(&ciphertext, b"123").unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    fn invalid_context() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .context_imprint();

        let plaintext = b"example plaintext";
        let ciphertext = cell.encrypt_with_context(&plaintext, b"123").unwrap();
        let recovered = cell.decrypt_with_context(&ciphertext, b"456").unwrap();

        assert_ne!(recovered, plaintext);
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .context_imprint();

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
        let seal = SecureCell::with_key("deep secret").unwrap().seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal.encrypt(&plaintext).unwrap();
        let recovered = seal.decrypt(&ciphertext).unwrap();

        assert_eq!(recovered, plaintext);
    }

    #[test]
    fn invalid_key() {
        let seal1 = SecureCell::with_key(b"deep secret").unwrap().seal();
        let seal2 = SecureCell::with_key(b"DEEP SECRET").unwrap().seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal1.encrypt(&plaintext).unwrap();
        let error = seal2.decrypt(&ciphertext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn invalid_context() {
        let seal = SecureCell::with_key(b"deep secret").unwrap().seal();

        let plaintext = b"example plaintext";
        let ciphertext = seal.encrypt_with_context(&plaintext, b"ctx1").unwrap();
        let error = seal.decrypt_with_context(&ciphertext, b"ctx2").unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_data() {
        let seal = SecureCell::with_key(b"deep secret").unwrap().seal();

        let plaintext = b"example plaintext";
        let mut ciphertext = seal.encrypt(&plaintext).unwrap();
        ciphertext[10] = !ciphertext[10];
        let error = seal.decrypt(&ciphertext).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }
}

mod seal_with_passphrase {
    use super::*;

    #[test]
    fn initialization() {
        assert!(SecureCell::with_passphrase("non-empty passphrase").is_ok());
        assert!(SecureCell::with_passphrase("").is_err());
    }

    #[test]
    fn roundtrip() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();
        let decrypted = cell.decrypt_with_context(&encrypted, &context).unwrap();

        assert_eq!(decrypted, message);
    }

    #[test]
    fn data_length_extension() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        assert!(encrypted.len() > message.len());
    }

    #[test]
    fn context_not_included() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context_short = b"Shteko budlanula bokra".as_ref();
        let context_long =
            b"Buffalo buffalo Buffalo buffalo buffalo buffalo Buffalo buffalo".as_ref();

        let encrypted_short = cell.encrypt_with_context(&message, &context_short).unwrap();
        let encrypted_long = cell.encrypt_with_context(&message, &context_long).unwrap();

        // Context is not (directly) included into encrypted message.
        assert_eq!(encrypted_short.len(), encrypted_long.len());
    }

    #[test]
    fn empty_context_handling() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        // encrypt is encrypt_with_context(..., )
        let encrypted_1 = cell.encrypt(&message).unwrap();
        let encrypted_2 = cell.encrypt_with_context(&message, &[]).unwrap();

        assert_eq!(cell.decrypt(&encrypted_1), Ok(message.to_vec()));
        assert_eq!(cell.decrypt(&encrypted_2), Ok(message.to_vec()));

        assert_eq!(
            cell.decrypt_with_context(&encrypted_1, &[]),
            Ok(message.to_vec())
        );
        assert_eq!(
            cell.decrypt_with_context(&encrypted_2, &[]),
            Ok(message.to_vec())
        );
    }

    #[test]
    fn passphrase_must_match() {
        let cell_a = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let cell_b = SecureCell::with_passphrase("loquaciousness")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell_a.encrypt(&message).unwrap();

        // You cannot use a different passphrase to decrypt data.
        assert!(cell_b.decrypt(&encrypted).is_err());

        // Only the correct passphrase will work.
        let decrypted = cell_a.decrypt(&encrypted).unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    fn context_must_match() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context_a = b"The jaws that bite, the claws that catch!".as_ref();
        let context_b = b"One, two! One, two! And through and through".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context_a).unwrap();

        // You cannot use a different context to decrypt data.
        assert!(cell.decrypt_with_context(&encrypted, &context_b).is_err());

        // Only the correct context will work.
        let decrypted = cell.decrypt_with_context(&encrypted, &context_a).unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    fn detects_corrupted_data() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        // Invert every odd byte, this will surely break the message.
        let mut corrupted = encrypted;
        for (i, b) in corrupted.iter_mut().enumerate() {
            if i % 2 == 1 {
                *b = !*b
            }
        }

        assert!(cell.decrypt(&corrupted).is_err());
    }

    #[test]
    fn detects_truncated_data() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        let truncated = &encrypted[..encrypted.len() - 1];

        assert!(cell.decrypt(truncated).is_err());
    }

    #[test]
    fn detects_extended_data() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        let mut extended = encrypted;
        extended.push(0);

        assert!(cell.decrypt(&extended).is_err());
    }

    #[test]
    fn empty_messages_not_allowed() {
        let cell = SecureCell::with_passphrase("sesquipedalian")
            .unwrap()
            .seal();

        assert!(cell.encrypt(&[]).is_err());
        assert!(cell.decrypt(&[]).is_err());
    }

    #[test]
    fn passphrases_not_compatible_with_keys() {
        let key = SymmetricKey::new();
        let cell_mk = SecureCell::with_key(&key).unwrap().seal();
        let cell_pw = SecureCell::with_passphrase(&key).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        // Passphrases are not keys, keys are not passphrases.
        let encrypted_mk = cell_mk.encrypt(&message).unwrap();
        assert!(cell_pw.decrypt(&encrypted_mk).is_err());

        let encrypted_pw = cell_pw.encrypt(&message).unwrap();
        assert!(cell_mk.decrypt(&encrypted_pw).is_err());
    }

    #[test]
    fn default_encoding_is_utf8() {
        // It's a key. -- No. Much more better! It is a drawing of a key.
        let cell_a = SecureCell::with_passphrase("🔑").unwrap().seal();
        let cell_b = SecureCell::with_passphrase(b"\xF0\x9F\x94\x91")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell_a.encrypt(&message).unwrap();
        let decrypted = cell_b.decrypt(&encrypted).unwrap();

        assert_eq!(decrypted, message);
    }

    #[test]
    fn supports_other_encodings() {
        // 🔑 in UTF-16BE
        let cell = SecureCell::with_passphrase(b"\xD8\x3D\xDD\x11")
            .unwrap()
            .seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        // Message encrypted by PyThemis
        let encrypted = b"\x00\x01\x01\x41\x0C\x00\x00\x00\x10\x00\x00\x00\x25\x00\x00\x00\x16\x00\x00\x00\xD9\xEF\x48\x0A\x41\x14\x0E\x93\xB1\x8C\x6C\x2A\xD6\x24\xF8\x7C\x44\x41\x6B\x53\xCE\x3F\x0A\x1D\x8D\xEC\x6B\x71\x40\x0D\x03\x00\x10\x00\xDE\x44\xDC\xA7\x31\x59\x3A\xF6\x14\x92\x04\xA2\xC1\x4C\xBE\x39\x2B\x20\x95\x44\x29\xA8\x75\x4C\x3F\x15\x99\x68\xF5\x98\x20\x63\x42\x89\x62\x55\x7C\x82\x61\x6B\x97\x07\x98\xE4\xE9\x37\x31\xD4\x9C\xB3\x05\x30\xC1".as_ref();
        let decrypted = cell.decrypt(&encrypted).unwrap();

        assert_eq!(decrypted, message);
    }
}

mod token_protect {
    use super::*;

    #[test]
    fn happy_path() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell.encrypt(&plaintext).unwrap();
        let recovered = cell.decrypt(&ciphertext, &token).unwrap();

        assert_eq!(recovered, plaintext);

        assert_eq!(plaintext.len(), ciphertext.len());
    }

    #[test]
    fn invalid_key() {
        let cell1 = SecureCell::with_key(b"deep secret")
            .unwrap()
            .token_protect();
        let cell2 = SecureCell::with_key(b"DEEP SECRET")
            .unwrap()
            .token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell1.encrypt(plaintext).unwrap();
        let error = cell2.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn invalid_context() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, token) = cell.encrypt_with_context(plaintext, b"123").unwrap();
        let error = cell
            .decrypt_with_context(&ciphertext, &token, b"456")
            .unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_data() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .token_protect();

        let plaintext = b"example plaintext";
        let (mut ciphertext, token) = cell.encrypt(&plaintext).unwrap();
        ciphertext[10] = !ciphertext[10];
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }

    #[test]
    fn corrupted_token() {
        let cell = SecureCell::with_key(b"deep secret")
            .unwrap()
            .token_protect();

        let plaintext = b"example plaintext";
        let (ciphertext, mut token) = cell.encrypt(&plaintext).unwrap();
        token[10] = !token[10];
        let error = cell.decrypt(&ciphertext, &token).unwrap_err();

        assert_eq!(error.kind(), ErrorKind::Fail);
    }
}
