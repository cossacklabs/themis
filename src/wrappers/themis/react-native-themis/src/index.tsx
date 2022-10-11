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



export function isBase64(str: String): boolean {
  const regex64 = /^([0-9a-zA-Z+/]{4})*(([0-9a-zA-Z+/]{2}==)|([0-9a-zA-Z+/]{3}=))?$/;
  return regex64.test(str as string);
}

export function string64(input: String): String {
  return Buffer.from(input).toString('base64')
}

const checkInput = (param: String, name: String) => {
  if (param === "" || param === undefined || param === null) {
    throw new Error(`Parameter ${name} can not be empty`);
  }
  return param;
}

const convertInputBase64 = (param: String, name: String): any => {
  checkInput(param, name);
  if (!isBase64(param)) {
    throw new Error(`Parameter ${name} is not base64 encoded`);
  }
  return Array.from(Buffer.from(param, 'base64'));
}

export function keyPair64(typeOfKey: any = KEYTYPE_EC): Promise<Object> {
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

  checkInput(plaintext, "plaintext"); // check plaintext is not empty
  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64"); // check symmetricKey64 is not empty and base64 encoded

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

  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64");
  const encrypted = convertInputBase64(encrypted64, "encrypted64");

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

  checkInput(plaintext, "plaintext");
  checkInput(passphrase, "passphrase");

  return new Promise((resolve, reject) => {
    Themis.secureCellSealWithPassphraseEncrypt(passphrase, plaintext, context, (encrypted: any) => {
      resolve(Buffer.from(new Uint8Array(encrypted)).toString("base64"))
    }, (error: any) => {
      reject(error)
    });
  });
};

export function secureCellSealWithPassphraseDecrypt64(
  passphrase: String,
  encrypted64: String,
  context: String = ""): Promise<string> {

  checkInput(passphrase, "passphrase");
  const encrypted = convertInputBase64(encrypted64, "encrypted64");

  return new Promise((resolve, reject) => {
    Themis.secureCellSealWithPassphraseDecrypt(passphrase, encrypted, context, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString());
    }, (error: any) => {
      reject(error);
    })
  });
}

export function secureCellTokenProtectEncrypt64(
  symmetricKey64: String,
  plaintext: String,
  context: String = ""): Promise<Object> {

  checkInput(plaintext, "plaintext");
  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64");

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

  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64");
  const encrypted = convertInputBase64(encrypted64, "encrypted64");
  const token = convertInputBase64(token64, "token64");

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

  checkInput(plaintext, "plaintext");
  checkInput(context, "context");
  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64");

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

  checkInput(context, "context");
  const symmetricKey = convertInputBase64(symmetricKey64, "symmetricKey64");
  const encrypted = convertInputBase64(encrypted64, "encrypted64");

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
  _publicKey64: String = ""): Promise<string> {

  checkInput(plaintext, "plaintext");
  const privateKey = convertInputBase64(privateKey64, "privateKey64");

  return new Promise((resolve, reject) => {
    Themis.secureMessageSign(plaintext, privateKey, (signed: any) => {
      resolve(Buffer.from(new Uint8Array(signed)).toString("base64"))
    }, (error: any) => {
      reject(error)
    })
  })
}

export function secureMessageVerify64(
  signed64: String,
  _privateKey64: String = "",
  publicKey64: String): Promise<string> {

  const signed = convertInputBase64(signed64, "signed64");
  const publicKey = convertInputBase64(publicKey64, "publicKey64");

  return new Promise((resolve, reject) => {
    Themis.secureMessageVerify(signed, publicKey, (verified: any) => {
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

  checkInput(plaintext, "plaintext");
  const privateKey = convertInputBase64(privateKey64, "privateKey64");
  const publicKey = convertInputBase64(publicKey64, "publicKey64");

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

  const encrypted = convertInputBase64(encrypted64, "encrypted64");
  const privateKey = convertInputBase64(privateKey64, "privateKey64");
  const publicKey = convertInputBase64(publicKey64, "publicKey64");

  return new Promise((resolve, reject) => {
    Themis.secureMessageDecrypt(encrypted, privateKey, publicKey, (decrypted: any) => {
      resolve(Buffer.from(new Uint8Array(decrypted)).toString())
    }, (error: any) => {
      reject(error)
    })
  })
}


/* Returns UUID in string value that corresponds to new comparator */
export function comparatorInit64(data64: String): Promise<string> {

  const data = convertInputBase64(data64, "data64");

  return new Promise((resolve, reject) => {
    Themis.initComparator(data, (comparator: string) => {
      resolve(comparator)
    }, (error: any) => {
      reject(error)
    })
  })
}

export function comparatorBegin(uuidStr: String): Promise<string> {
  checkInput(uuidStr, "uuidStr");

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

  checkInput(uuidStr, "uuidStr");
  const data = convertInputBase64(data64, "data64");

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