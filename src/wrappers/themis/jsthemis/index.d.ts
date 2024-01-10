export declare class KeyPair {
    constructor(privateKey?: Uint8Array, publicKey?: Uint8Array);
    public(): Uint8Array;
    private(): Uint8Array;
}

export declare class SymmetricKey {
    constructor();
}

export declare class SecureCellSeal {
    static withKey(key: SymmetricKey): SecureCellSeal;
    static withPassphrase(passphrase: string): SecureCellSeal;
    encrypt(plaintext: Uint8Array, context?: Uint8Array): Uint8Array;
    decrypt(encrypted: Uint8Array, context?: Uint8Array): Uint8Array;
}

export declare interface SecureCellTokenProtectResult {
    data: Uint8Array;
    token: Uint8Array;
}

export declare class SecureCellTokenProtect {
    constructor(key: SymmetricKey);
    static withKey(key: SymmetricKey): SecureCellTokenProtect;
    encrypt(plaintext: Uint8Array, context?: Uint8Array): SecureCellTokenProtectResult;
    decrypt(encrypted: Uint8Array, token: Uint8Array, context?: Uint8Array): Uint8Array;
}

export declare class SecureCellContextImprint {
    static withKey(key: SymmetricKey): SecureCellContextImprint;
    encrypt(plaintext: Uint8Array, context: Uint8Array): Uint8Array;
    decrypt(encrypted: Uint8Array, context: Uint8Array): Uint8Array;
}

export declare class SecureMessage {
    constructor(privateKey: Uint8Array | null, publicKey: Uint8Array | null);
    sign(message: Uint8Array): Uint8Array;
    verify(message: Uint8Array): Uint8Array;
    encrypt(message: Uint8Array): Uint8Array;
    decrypt(message: Uint8Array): Uint8Array;
}

type GetPublicKeyCallback = (peerID: Uint8Array) => Uint8Array | null;

export declare class SecureSession {
    constructor(peerID: Uint8Array, privateKey: Uint8Array, getPublicKeyCallback: GetPublicKeyCallback);
    isEstablished(): boolean;
    connectRequest(): Uint8Array;
    wrap(message: Uint8Array): Uint8Array;
    unwrap(message: Uint8Array): Uint8Array;
}

export declare class SecureComparator {
    constructor(sharedSecret: Uint8Array);
    beginCompare(): Uint8Array;
    proceedCompare(request: Uint8Array): Uint8Array;
    isCompareComplete(): boolean;
    isMatch(): boolean;
}
