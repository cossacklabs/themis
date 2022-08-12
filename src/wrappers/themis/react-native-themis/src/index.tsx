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
  if (typeOfKey !== KEYTYPE_RSA && typeOfKey !== KEYTYPE_EC) {
    throw new Error('Invalid key type');
  }

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

export function secureCellSealWithSymmetricKeyEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String = ""): Promise<string> {

  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }
  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellSealWithSymmetricKeyEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
};

export function secureCellSealWithSymmetricKeyDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  context: String = ""): Promise<string> {

  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }

  if (encrypted64 === "" || encrypted64 === undefined || encrypted64 === null) {
    throw new Error("Parameter encrypted64 can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellSealWithSymmetricKeyDecrypt(symmetricKey, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
};

export function secureCellSealWithPassphraseEncrypt64(
  passphrase: String,
  plaintext: String,
  context: String = ""): Promise<string> {

  if (passphrase === "" || passphrase === undefined || passphrase === null) {
    throw new Error("Parameter passphrase can not be empty");
  }
  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }

  return new Promise((resolve) => {
    Themis.secureCellSealWithPassphraseEncrypt(passphrase, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    });
  });
};

export function secureCellSealWithPassphraseDecrypt64(
  passphrase: String,
  encrypted64: String,
  context: String = ""): Promise<string> {

  if (passphrase === "" || passphrase === undefined || passphrase === null) {
    throw new Error("Parameter passphrase can not be empty");
  }

  if (encrypted64 === "" || encrypted64 === undefined || encrypted64 === null) {
    throw new Error("Parameter encrypted64 can not be empty");
  }

  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellSealWithPassphraseDecrypt(passphrase, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  });
}

export function secureCellTokenProtectEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String = ""): Promise<Object> {

  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }
  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellTokenProtectEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
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

export function secureCellTokenProtectDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  token64: String,
  context: String = ""): Promise<string> {

  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }
  if (encrypted64 === "" || encrypted64 === undefined || encrypted64 === null) {
    throw new Error("Parameter encrypted64 can not be empty");
  }
  if (token64 === "" || token64 === undefined || token64 === null) {
    throw new Error("Parameter token64 can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));
  const token = Array.from(Buffer.from(token64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellTokenProtectDecrypt(symmetricKey, encrypted, token, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
}

// context imprint encrypt and decrypt
export function secureCellContextImprintEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String): Promise<string> {

  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }
  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }
  if (context === "" || context === undefined || context === null) {
    throw new Error("Parameter context can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  return new Promise((resolve, reject) => {
    Themis.secureCellContextImprintEncrypt(symmetricKey, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  });
}

export function secureCellContextImprintDecrypt64(
  symmetricKey64: String,
  encrypted64: String,
  context: String): Promise<string> {

  if (symmetricKey64 === "" || symmetricKey64 === undefined || symmetricKey64 === null) {
    throw new Error("Parameter symmetricKey64 can not be empty");
  }
  if (encrypted64 === "" || encrypted64 === undefined || encrypted64 === null) {
    throw new Error("Parameter encrypted64 can not be empty");
  }
  if (context === "" || context === undefined || context === null) {
    throw new Error("Parameter context can not be empty");
  }

  const symmetricKey = Array.from(Buffer.from(symmetricKey64, 'base64'));
  const encrypted = Array.from(Buffer.from(encrypted64, 'base64'));

  return new Promise((resolve, reject) => {
    Themis.secureCellContextImprintDecrypt(symmetricKey, encrypted, context, (decrypted: any) => {
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

  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }
  if (privateKey64 === "" || privateKey64 === undefined || privateKey64 === null) {
    throw new Error("Parameter privateKey64 can not be empty");
  }

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

  if (signed64 === "" || signed64 === undefined || signed64 === null) {
    throw new Error("Parameter signed64 can not be empty");
  }
  if (publicKey64 === "" || publicKey64 === undefined || publicKey64 === null) {
    throw new Error("Parameter publicKey64 can not be empty");
  }

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

  if (plaintext === "" || plaintext === undefined || plaintext === null) {
    throw new Error("Parameter plaintext can not be empty");
  }
  if (privateKey64 === "" || privateKey64 === undefined || privateKey64 === null) {
    throw new Error("Parameter privateKey64 can not be empty");
  }
  if (publicKey64 === "" || publicKey64 === undefined || publicKey64 === null) {
    throw new Error("Parameter publicKey64 can not be empty");
  }


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

  if (encrypted64 === "" || encrypted64 === undefined || encrypted64 === null) {
    throw new Error("Parameter encrypted64 can not be empty");
  }
  if (privateKey64 === "" || privateKey64 === undefined || privateKey64 === null) {
    throw new Error("Parameter privateKey64 can not be empty");
  }
  if (publicKey64 === "" || publicKey64 === undefined || publicKey64 === null) {
    throw new Error("Parameter publicKey64 can not be empty");
  }


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

  if (data64 === "" || data64 === undefined || data64 === null) {
    throw new Error("Parameter data64 can not be empty");
  }

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

  if (uuidStr === "" || uuidStr === undefined || uuidStr === null) {
    throw new Error("Parameter uuidStr can not be empty");
  }

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

  if (uuidStr === "" || uuidStr === undefined || uuidStr === null) {
    throw new Error("Parameter uuidStr can not be empty");
  }
  if (data64 === "" || data64 === undefined || data64 === null) {
    throw new Error("Parameter data64 can not be empty");
  }


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