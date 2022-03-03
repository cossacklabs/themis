import { NativeModules, Platform } from 'react-native';
import { Buffer } from 'buffer';
const LINKING_ERROR = `The package 'react-native-themis' doesn't seem to be linked. Make sure: \n\n` +
    Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
    '- You rebuilt the app after installing the package\n' +
    '- You are not using Expo managed workflow\n';
const Themis = NativeModules.Themis
    ? NativeModules.Themis
    : new Proxy({}, {
        get() {
            throw new Error(LINKING_ERROR);
        },
    });
export const { COMPARATOR_NOT_READY, COMPARATOR_NOT_MATCH, COMPARATOR_MATCH, COMPARATOR_ERROR, KEYTYPE_RSA, KEYTYPE_EC } = Themis.getConstants();
export function keyPair64(typeOfKey) {
    return new Promise((resolve) => {
        Themis.keyPair(typeOfKey, (pair) => {
            const pvtKey64 = Buffer.from(new Uint8Array(pair.private)).toString("base64");
            const pubKey64 = Buffer.from(new Uint8Array(pair.public)).toString("base64");
            resolve({
                private64: pvtKey64,
                public64: pubKey64,
            });
        });
    });
}
;
export function symmetricKey64() {
    return new Promise((resolve) => {
        Themis.symmetricKey((key) => {
            resolve(Buffer.from(new Uint8Array(key)).toString("base64"));
        });
    });
}
;
export function secureCellSealWithSymmetricKeyEncrypt64(symmetricKey64, plaintext, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellSealWithSymmetricKeyEncrypt(symmetricKey, plaintext, context, (encrypted) => {
            resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"));
        }, (error) => {
            reject(error);
        });
    });
}
;
export function secureCellSealWithSymmetricKeyDecrypt64(symmetricKey64, encrypted64, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellSealWithSymmetricKeyDecrypt(symmetricKey, encrypted, context, (decrypted) => {
            resolve(Buffer.from(new Uint8Array(decrypted)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
;
export function secureCellSealWithPassphraseEncrypt64(passphrase, plaintext, context) {
    return new Promise((resolve) => {
        Themis.secureCellSealWithPassphraseEncrypt(passphrase, plaintext, context, (encrypted) => {
            resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"));
        });
    });
}
;
export function secureCellSealWithPassphraseDecrypt64(passphrase, encrypted64, context) {
    const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellSealWithPassphraseDecrypt(passphrase, encrypted, context, (decrypted) => {
            resolve(Buffer.from(new Uint8Array(decrypted)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
export function secureCellTokenProtectEncrypt64(symmetricKey64, plaintext, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellTokenProtectEncrypt(symmetricKey, plaintext, context, (encrypted) => {
            const data = Buffer.from(new Uint8Array(encrypted.encrypted)).toString("base64");
            const token = Buffer.from(new Uint8Array(encrypted.token)).toString("base64");
            resolve({
                encrypted64: data,
                token64: token
            });
        }, (error) => {
            reject(error);
        });
    });
}
export function secureCellTokenProtectDecrypt64(symmetricKey64, encrypted64, token64, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
    const token = Array.from(Buffer.from(token64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellTokenProtectDecrypt(symmetricKey, encrypted, token, context, (decrypted) => {
            resolve(Buffer.from(new Uint8Array(decrypted)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
// context imprint encrypt and decrypt 
export function secureCellContextImprintEncrypt64(symmetricKey64, plaintext, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellContextImprintEncrypt(symmetricKey, plaintext, context, (encrypted) => {
            resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"));
        }, (error) => {
            reject(error);
        });
    });
}
export function secureCellContextImprintDecrypt64(symmetricKey64, encrypted64, context) {
    const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
    const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureCellContextImprintDecrypt(symmetricKey, encrypted, context, (decrypted) => {
            resolve(Buffer.from(new Uint8Array(decrypted)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
// secure message sign and verify 
export function secureMessageSign64(plaintext, privateKey64, publicKey64) {
    const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
    const publicKey = publicKey64 !== null && publicKey64 !== "" ?
        Array.from(Buffer.from(publicKey64, 'base64')) : null;
    return new Promise((resolve, reject) => {
        Themis.secureMessageSign(plaintext, privateKey, publicKey, (signed) => {
            resolve(Buffer.from(new Uint8Array(signed)).toString("base64"));
        }, (error) => {
            reject(error);
        });
    });
}
export function secureMessageVerify64(signed64, privateKey64, publicKey64) {
    const signed = Array.from(Buffer.from(signed64, 'base64'));
    const privateKey = privateKey64 !== null && privateKey64 !== "" ?
        Array.from(Buffer.from(privateKey64, 'base64')) : null;
    const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureMessageVerify(signed, privateKey, publicKey, (verified) => {
            resolve(Buffer.from(new Uint8Array(verified)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
// secure message encrypt and decrypt 
export function secureMessageEncrypt64(plaintext, privateKey64, publicKey64) {
    const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
    const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureMessageEncrypt(plaintext, privateKey, publicKey, (encrypted) => {
            resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"));
        }, (error) => {
            reject(error);
        });
    });
}
export function secureMessageDecrypt64(encrypted64, privateKey64, publicKey64) {
    const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
    const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
    const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.secureMessageDecrypt(encrypted, privateKey, publicKey, (decrypted) => {
            resolve(Buffer.from(new Uint8Array(decrypted)).toString());
        }, (error) => {
            reject(error);
        });
    });
}
export function string64(input) {
    return Buffer.from(input).toString('base64');
}
/* Returns UUID in string value that corresponds to new comparator */
export function comparatorInit64(data64) {
    const data = Array.from(Buffer.from(data64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.initComparator(data, (comparator) => {
            resolve(comparator);
        }, (error) => {
            reject(error);
        });
    });
}
export function comparatorBegin(uuidStr) {
    return new Promise((resolve, reject) => {
        Themis.beginCompare(uuidStr, (data) => {
            resolve(Buffer.from(new Uint8Array(data)).toString("base64"));
        }, (error) => {
            reject(error);
        });
    });
}
/* Returns next part of data and current status */
export function comparatorProceed64(uuidStr, data64) {
    const data = Array.from(Buffer.from(data64, 'base64'));
    return new Promise((resolve, reject) => {
        Themis.proceedCompare(uuidStr, data, (nextData, status) => {
            const nextdata64 = Buffer.from(new Uint8Array(nextData)).toString("base64");
            resolve({
                data64: nextdata64,
                status: status
            });
        }, (error) => {
            reject(error);
        });
    });
}
