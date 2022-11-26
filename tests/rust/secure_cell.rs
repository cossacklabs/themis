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

use themis::keys::SymmetricKey;
use themis::secure_cell::SecureCell;

mod context_imprint {
    use super::*;

    #[test]
    fn initialization() {
        assert!(SecureCell::with_key(SymmetricKey::new()).is_ok());
        assert!(SecureCell::with_key(&[]).is_err());
    }

    #[test]
    fn roundtrip() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();
        let decrypted = cell.decrypt_with_context(&encrypted, &context).unwrap();

        assert_eq!(decrypted, message);
    }

    #[test]
    fn data_length_preservation() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();

        assert_eq!(encrypted.len(), message.len());
    }

    #[test]
    fn context_not_included() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
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
    fn key_must_match() {
        let cell_a = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let cell_b = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell_a.encrypt_with_context(&message, &context).unwrap();

        // Context Imprint mode does not validate message data so using an incorrect key
        // will successfully return garbage output.
        let decrypted_incorrect = cell_b.decrypt_with_context(&encrypted, &context).unwrap();
        assert_ne!(decrypted_incorrect, message);
        assert_ne!(decrypted_incorrect, encrypted);

        // Only the correct key will work.
        let decrypted_correct = cell_a.decrypt_with_context(&encrypted, &context).unwrap();
        assert_eq!(decrypted_correct, message);
    }

    #[test]
    fn context_must_match() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context_a = b"The jaws that bite, the claws that catch!".as_ref();
        let context_b = b"One, two! One, two! And through and through".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context_a).unwrap();

        // Context Imprint mode does not validate message data so using an incorrect context
        // will successfully return garbage output.
        let decrypted_incorrect = cell.decrypt_with_context(&encrypted, &context_b).unwrap();
        assert_ne!(decrypted_incorrect, message);
        assert_ne!(decrypted_incorrect, encrypted);

        // Only the correct context will work.
        let decrypted_correct = cell.decrypt_with_context(&encrypted, &context_a).unwrap();
        assert_eq!(decrypted_correct, message);
    }

    #[test]
    fn not_detects_corrupted_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();

        // Invert every odd byte, this will surely break the message.
        let mut corrupted = encrypted;
        for (i, b) in corrupted.iter_mut().enumerate() {
            if i % 2 == 1 {
                *b = !*b
            }
        }

        // Decrypts successfully but the content is garbage.
        let decrypted = cell.decrypt_with_context(&corrupted, &context).unwrap();
        assert_ne!(decrypted, message);
    }

    #[test]
    fn not_detects_truncated_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();

        let truncated = &encrypted[..encrypted.len() - 1];

        // Decrypts successfully but the content is garbage.
        let decrypted = cell.decrypt_with_context(&truncated, &context).unwrap();
        assert_ne!(decrypted, message);
    }

    #[test]
    fn detects_extended_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();

        let mut extended = encrypted;
        extended.push(0);

        // Decrypts successfully but the content is garbage.
        let decrypted = cell.decrypt_with_context(&extended, &context).unwrap();
        assert_ne!(decrypted, message);
    }

    #[test]
    fn empty_input_not_allowed() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .context_imprint();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        // With Context Imprint the context cannot be empty.
        assert!(cell.encrypt_with_context(&message, &[]).is_err());
        assert!(cell.encrypt_with_context(&[], &context).is_err());

        assert!(cell.decrypt_with_context(&message, &[]).is_err());
        assert!(cell.decrypt_with_context(&[], &context).is_err());
    }
}

mod seal {
    use super::*;

    #[test]
    fn initialization() {
        assert!(SecureCell::with_key(SymmetricKey::new()).is_ok());
        assert!(SecureCell::with_key(&[]).is_err());
    }

    #[test]
    fn roundtrip() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let encrypted = cell.encrypt_with_context(&message, &context).unwrap();
        let decrypted = cell.decrypt_with_context(&encrypted, &context).unwrap();

        assert_eq!(decrypted, message);
    }

    #[test]
    fn data_length_extension() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        assert!(encrypted.len() > message.len());
    }

    #[test]
    fn context_not_included() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
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
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        // encrypt(...) is encrypt_with_context(..., &[])
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
    fn key_must_match() {
        let cell_a = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let cell_b = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell_a.encrypt(&message).unwrap();

        // You cannot use a different key to decrypt data.
        assert!(cell_b.decrypt(&encrypted).is_err());

        // Only the correct key will work.
        let decrypted = cell_a.decrypt(&encrypted).unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    fn context_must_match() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
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
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
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
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        let truncated = &encrypted[..encrypted.len() - 1];

        assert!(cell.decrypt(truncated).is_err());
    }

    #[test]
    fn detects_extended_data() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let encrypted = cell.encrypt(&message).unwrap();

        let mut extended = encrypted;
        extended.push(0);

        assert!(cell.decrypt(&extended).is_err());
    }

    #[test]
    fn empty_messages_not_allowed() {
        let cell = SecureCell::with_key(SymmetricKey::new()).unwrap().seal();

        assert!(cell.encrypt(&[]).is_err());
        assert!(cell.decrypt(&[]).is_err());
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
        let encrypted = b"\x00\x01\x01\x41\x0C\x00\x00\x00\x10\x00\x00\x00\x25\x00\x00\x00\x16\x00\x00\x00\x78\x98\x93\x12\xC9\x60\x1E\x22\xD7\xCB\x47\x06\xC9\xEC\x46\xB5\xB5\x9A\xFC\xC8\x3F\x06\x8F\x5B\xBE\x9F\x66\xA6\x40\x0D\x03\x00\x10\x00\x1C\x6D\x16\xFF\x39\xB9\x13\xDF\xC4\x41\x56\x31\x17\xF3\xC4\x05\x28\x15\x13\xA6\x74\x29\x0B\xF0\x5A\xFB\xAC\xD9\x79\x35\x7E\xBA\xD0\x8F\x8C\xA6\x9E\x4E\x83\x2A\x12\xBA\xC7\x59\xAA\xF6\xDF\x62\x8E\xCE\x31\x7C\xCF".as_ref();
        let decrypted = cell.decrypt(&encrypted).unwrap();

        assert_eq!(decrypted, message);
    }
}

mod token_protect {
    use super::*;

    #[test]
    fn initialization() {
        assert!(SecureCell::with_key(SymmetricKey::new()).is_ok());
        assert!(SecureCell::with_key(&[]).is_err());
    }

    #[test]
    fn roundtrip() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context = b"...and a toilet seat cover!".as_ref();

        let (encrypted, token) = cell.encrypt_with_context(&message, &context).unwrap();
        let decrypted = cell
            .decrypt_with_context(&encrypted, &token, &context)
            .unwrap();

        assert_eq!(decrypted, message);
    }

    #[test]
    fn data_length_preservation() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        assert_eq!(encrypted.len(), message.len());
        assert!(!token.is_empty());
    }

    #[test]
    fn context_not_included() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context_short = b"Shteko budlanula bokra".as_ref();
        let context_long =
            b"Buffalo buffalo Buffalo buffalo buffalo buffalo Buffalo buffalo".as_ref();

        let (encrypted_short, token_short) =
            cell.encrypt_with_context(&message, &context_short).unwrap();
        let (encrypted_long, token_long) =
            cell.encrypt_with_context(&message, &context_long).unwrap();

        // Context is not (directly) included into encrypted message.
        assert_eq!(encrypted_short.len(), encrypted_long.len());
        assert_eq!(token_short.len(), token_long.len());
    }

    #[test]
    fn empty_context_handling() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        // encrypt(...) is encrypt_with_context(..., &[])
        let (encrypted_1, token_1) = cell.encrypt(&message).unwrap();
        let (encrypted_2, token_2) = cell.encrypt_with_context(&message, &[]).unwrap();

        assert_eq!(cell.decrypt(&encrypted_1, &token_1), Ok(message.to_vec()));
        assert_eq!(cell.decrypt(&encrypted_2, &token_2), Ok(message.to_vec()));

        assert_eq!(
            cell.decrypt_with_context(&encrypted_1, &token_1, &[]),
            Ok(message.to_vec())
        );
        assert_eq!(
            cell.decrypt_with_context(&encrypted_2, &token_2, &[]),
            Ok(message.to_vec())
        );
    }

    #[test]
    fn key_must_match() {
        let cell_a = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let cell_b = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell_a.encrypt(&message).unwrap();

        // You cannot use a different key to decrypt data.
        assert!(cell_b.decrypt(&encrypted, &token).is_err());

        // Only the correct key will work.
        let decrypted = cell_a.decrypt(&encrypted, &token).unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    fn token_must_match() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted_1, token_1) = cell.encrypt(&message).unwrap();
        let (encrypted_2, token_2) = cell.encrypt(&message).unwrap();

        // You cannot use a different token to decrypt data, even the same original data.
        assert!(cell.decrypt(&encrypted_1, &token_2).is_err());
        assert!(cell.decrypt(&encrypted_2, &token_1).is_err());

        // Only the matching token will work.
        let decrypted_1 = cell.decrypt(&encrypted_1, &token_1).unwrap();
        let decrypted_2 = cell.decrypt(&encrypted_2, &token_2).unwrap();
        assert_eq!(decrypted_1, message);
        assert_eq!(decrypted_2, message);
    }

    #[test]
    fn context_must_match() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();
        let context_a = b"The jaws that bite, the claws that catch!".as_ref();
        let context_b = b"One, two! One, two! And through and through".as_ref();

        let (encrypted, token) = cell.encrypt_with_context(&message, &context_a).unwrap();

        // You cannot use a different context to decrypt data.
        assert!(cell
            .decrypt_with_context(&encrypted, &token, &context_b)
            .is_err());

        // Only the correct context will work.
        let decrypted = cell
            .decrypt_with_context(&encrypted, &token, &context_a)
            .unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    fn detects_corrupted_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        // Invert every odd byte, this will surely break the message.
        let mut corrupted_data = encrypted;
        for (i, b) in corrupted_data.iter_mut().enumerate() {
            if i % 2 == 1 {
                *b = !*b
            }
        }

        assert!(cell.decrypt(&corrupted_data, &token).is_err());
    }

    #[test]
    fn detects_truncated_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        let truncated_data = &encrypted[..encrypted.len() - 1];

        assert!(cell.decrypt(&truncated_data, &token).is_err());
    }

    #[test]
    fn detects_extended_data() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        let mut extended_data = encrypted;
        extended_data.push(0);

        assert!(cell.decrypt(&extended_data, &token).is_err());
    }

    #[test]
    // FIXME(ilammy, 2020-05-25): avoid capacity allocation panics (T1649)
    // This tests panics on 32-bit architectures due to size overflow.
    // The implementation needs to use Vec::try_reserve instead of Vec::reserve
    // when it becomes available in stable Rust.
    #[cfg_attr(target_pointer_width = "32", ignore)]
    fn detects_corrupted_token() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        // Invert every odd byte, this will surely break the token.
        let mut corrupted_token = token;
        for (i, b) in corrupted_token.iter_mut().enumerate() {
            if i % 2 == 1 {
                *b = !*b
            }
        }

        assert!(cell.decrypt(&encrypted, &corrupted_token).is_err());
    }

    #[test]
    fn detects_truncated_token() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        let truncated_token = &token[..token.len() - 1];

        assert!(cell.decrypt(&encrypted, &truncated_token).is_err());
    }

    #[test]
    fn detects_extended_token() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        let mut extended_token = token;
        extended_token.push(0);

        // Current implementation of Secure Cell allows the token to be overlong.
        // Extra data is simply ignored.
        let decrypted = cell.decrypt(&encrypted, &extended_token).unwrap();
        assert_eq!(decrypted, message);
    }

    #[test]
    // FIXME(ilammy, 2020-05-25): avoid capacity allocation panics (T1649)
    // This tests panics on 32-bit architectures due to size overflow.
    // The implementation needs to use Vec::try_reserve instead of Vec::reserve
    // when it becomes available in stable Rust.
    #[cfg_attr(target_pointer_width = "32", ignore)]
    fn detects_data_token_swap() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();
        let message = b"Colorless green ideas sleep furiously".as_ref();

        let (encrypted, token) = cell.encrypt(&message).unwrap();

        assert!(cell.decrypt(&token, &encrypted).is_err());
    }

    #[test]
    fn empty_input_not_allowed() {
        let cell = SecureCell::with_key(SymmetricKey::new())
            .unwrap()
            .token_protect();

        assert!(cell.encrypt(&[]).is_err());

        let message = b"Colorless green ideas sleep furiously".as_ref();
        let (encrypted, token) = cell.encrypt(&message).unwrap();

        assert!(cell.decrypt(&encrypted, &[]).is_err());
        assert!(cell.decrypt(&[], &token).is_err());
    }
}
