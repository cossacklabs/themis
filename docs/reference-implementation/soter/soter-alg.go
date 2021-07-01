package soter

// AlgorithmID encodes symmetric algorithm parameters for Soter.
type AlgorithmID uint32

const (
	algorithmMask   = 0xF0000000
	algorithmOffset = 28
	kdfMask         = 0x0F000000
	kdfOffset       = 24
	paddingMask     = 0x000F0000
	paddingOffset   = 16
	keyLengthMask   = 0x00000FFF
	keyLengthOffset = 0
)

// SymmetricAlgorithm indicates symmetric encryption algorithm in AlgorithmID.
type SymmetricAlgorithm int

// Supported SymmetricAlgorithm values.
const (
	AesECB SymmetricAlgorithm = 0x1
	AesCBC SymmetricAlgorithm = 0x2
	AesXTS SymmetricAlgorithm = 0x3
	AesGCM SymmetricAlgorithm = 0x4
)

// KeyDerivationFunction indicates key derivation function in AlgorithmID.
type KeyDerivationFunction int

// Supported KeyDerivationFunction values.
const (
	NoKDF            KeyDerivationFunction = 0x0
	PBKDF2HmacSha256 KeyDerivationFunction = 0x1
)

// PaddingAlgorithm indicates padding algorithm in AlgorithmID.
type PaddingAlgorithm int

// Supported PaddingAlgorithm values.
const (
	NoPadding    PaddingAlgorithm = 0x0
	PKCS7Padding PaddingAlgorithm = 0x1
)

// MakeAlgorithmID constructs algorithm ID from components.
func MakeAlgorithmID(algorithm SymmetricAlgorithm, kdf KeyDerivationFunction, padding PaddingAlgorithm, keyBits int) AlgorithmID {
	var value uint32
	value |= uint32(algorithm) << algorithmOffset
	value |= uint32(kdf) << kdfOffset
	value |= uint32(padding) << paddingOffset
	value |= uint32(keyBits) << keyLengthOffset
	return AlgorithmID(value)
}

// Algorithm returns symmetric algorithm component.
func (id AlgorithmID) Algorithm() SymmetricAlgorithm {
	return SymmetricAlgorithm((id & algorithmMask) >> algorithmOffset)
}

// KDF returns key derivation function.
func (id AlgorithmID) KDF() KeyDerivationFunction {
	return KeyDerivationFunction((id & kdfMask) >> kdfOffset)
}

// Padding returns padding algorithm.
func (id AlgorithmID) Padding() PaddingAlgorithm {
	return PaddingAlgorithm((id & paddingMask) >> paddingOffset)
}

// KeyBits returns size of the key in bits.
func (id AlgorithmID) KeyBits() int {
	return int((id & keyLengthMask) >> keyLengthOffset)
}
