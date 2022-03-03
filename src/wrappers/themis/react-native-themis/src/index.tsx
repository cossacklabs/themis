import { NativeModules, Platform } from 'react-native';
import { Buffer } from 'buffer';


const LINKING_ERROR =
  `The package 'react-native-themis' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo managed workflow\n';

const Themis = NativeModules.Themis
  ? NativeModules.Themis
  : new Proxy(
    {},
    {
      get() {
        throw new Error(LINKING_ERROR);
      },
    }
  );

export const {
  COMPARATOR_NOT_READY,
  COMPARATOR_NOT_MATCH,
  COMPARATOR_MATCH,
  COMPARATOR_ERROR,
  KEYTYPE_RSA,
  KEYTYPE_EC } = Themis.getConstants()


export function keyPair64(typeOfKey: any): Promise<Object> {
  return new Promise((resolve) => {
    Themis.keyPair(typeOfKey, (pair: any) => {
      const pvtKey64 = Buffer.from(new Uint8Array(pair.private)).toString("base64");
      const pubKey64 = Buffer.from(new Uint8Array(pair.public)).toString("base64");
      resolve({
        private64: pvtKey64,
        public64: pubKey64,
      });
    })
  })
};

export function symmetricKey64(): Promise<string> {
  return new Promise((resolve) => {
    Themis.symmetricKey((key: any) => {
      resolve(Buffer.from(new Uint8Array(key)).toString("base64"));
    })
  })
};

export function secureSealWithSymmetricKeyEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String): Promise<string> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureSealWithSymmetricKeyEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
};

export function secureSealWithSymmetricKeyDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  context: String): Promise<string> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureSealWithSymmetricKeyDecrypt(symmetricKey, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
};

export function secureSealWithPassphraseEncrypt64(
  passphrase: String,
  plaintext: String,
  context: String): Promise<string> {

  return new Promise((resolve) => {
    Themis.secureSealWithPassphraseEncrypt(passphrase, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    });
  });
};

export function secureSealWithPassphraseDecrypt64(
  passphrase: String,
  encrypted64: String,
  context: String): Promise<string> {

  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureSealWithPassphraseDecrypt(passphrase, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  });
}

export function tokenProtectEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String): Promise<Object> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.tokenProtectEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
      const data = Buffer.from(new Uint8Array(encrypted.encrypted)).toString("base64")
      const token = Buffer.from(new Uint8Array(encrypted.token)).toString("base64")
      resolve({
        encrypted64: data,
        token64: token
      })
    }, (error: any) => {
      reject(error)
    })
  })
}

export function tokenProtectDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  token64: String,
  context: String): Promise<string> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
  const token = Array.from(Buffer.from(token64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.tokenProtectDecrypt(symmetricKey, encrypted, token, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
}

// context imprint encrypt and decrypt 
export function contextImprintEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String): Promise<string> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  return new Promise((resolve, reject) => {
    Themis.contextImprintEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  });
}

export function contextImprintDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  context: String): Promise<string> {

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.contextImprintDecrypt(symmetricKey, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  });
}

// secure message sign and verify 
export function secureMessageSign64(
  plaintext: String,
  privateKey64: String,
  publicKey64: String): Promise<string> {

  const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
  const publicKey = publicKey64 !== null && publicKey64 !== "" ?
    Array.from(Buffer.from(publicKey64, 'base64')) : null;

  return new Promise((resolve, reject) => {
    Themis.secureMessageSign(plaintext, privateKey, publicKey, (signed: any) => {
      resolve(Buffer.from(new Uint8Array(signed)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
}

export function secureMessageVerify64(
  signed64: String,
  privateKey64: String,
  publicKey64: String): Promise<string> {

  const signed = Array.from(Buffer.from(signed64, 'base64'));
  const privateKey = privateKey64 !== null && privateKey64 !== "" ?
    Array.from(Buffer.from(privateKey64, 'base64')) : null;
  const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureMessageVerify(signed, privateKey, publicKey, (verified: any) => {
      resolve(Buffer.from(new Uint8Array(verified)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
}

// secure message encrypt and decrypt 
export function secureMessageEncrypt64(
  plaintext: String,
  privateKey64: String,
  publicKey64: String): Promise<string> {

  const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
  const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureMessageEncrypt(plaintext, privateKey, publicKey, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
}

export function secureMessageDecrypt64(
  encrypted64: String,
  privateKey64: String,
  publicKey64: String): Promise<string> {

  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
  const privateKey = Array.from(Buffer.from(privateKey64, 'base64'));
  const publicKey = Array.from(Buffer.from(publicKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureMessageDecrypt(encrypted, privateKey, publicKey, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
}

export function string64(input: String): String {
  return Buffer.from(input).toString('base64')
}

/* Returns UUID in string value that corresponds to new comparator */
export function comparatorInit64(data64: String): Promise<string> {
  const data = Array.from(Buffer.from(data64, 'base64'))
  return new Promise((resolve, reject) => {
    Themis.initComparator(data, (comparator: string) => {
      resolve(comparator)
    }, (error: any) => {
      reject(error)
    })
  })
}

export function comparatorBegin(uuidStr: String): Promise<string> {
  return new Promise((resolve, reject) => {
    Themis.beginCompare(uuidStr, (data: any) => {
      resolve(Buffer.from(new Uint8Array(data)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
}

/* Returns next part of data and current status */
export function comparatorProceed64(
  uuidStr: String,
  data64: String): Promise<Object> {

  const data = Array.from(Buffer.from(data64, 'base64'))
  return new Promise((resolve, reject) => {
    Themis.proceedCompare(uuidStr, data, (nextData: any, status: Number) => {
      const nextdata64 = Buffer.from(new Uint8Array(nextData)).toString("base64")
      resolve({
        data64: nextdata64,
        status: status
      })
    }, (error: any) => {
      reject(error)
    })
  })

}