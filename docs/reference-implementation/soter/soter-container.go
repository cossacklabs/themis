package soter

import (
	"bytes"
	"encoding/binary"
	"hash/crc32"
	"math"
)

// SoterContainer is a lightweight data container with basic integrity checks.
type SoterContainer struct {
	TagBytes [4]byte
	Payload  []byte
}

// Serialize this container, append it to the given buffer, and return it.
func (container *SoterContainer) Serialize(buffer []byte) []byte {
	buffer = append(buffer, container.TagBytes[:]...)
	buffer = append(buffer, container.lengthBytes()...)
	buffer = append(buffer, container.Checksum()...)
	buffer = append(buffer, container.Payload...)
	return buffer
}

func (container *SoterContainer) lengthBytes() []byte {
	// Length field includes the length of the header too
	const maxPayloadLength = math.MaxUint32 - 12
	if len(container.Payload) > maxPayloadLength {
		panic("Soter container payload too long")
	}
	length := make([]byte, 4)
	binary.BigEndian.PutUint32(length, uint32(12+len(container.Payload)))
	return length
}

// Checksum computes CRC checksum of this container.
func (container *SoterContainer) Checksum() []byte {
	// CRC-32C
	crc := crc32.New(crc32.MakeTable(crc32.Castagnoli))
	crc.Write(container.TagBytes[:])
	crc.Write(container.lengthBytes())
	crc.Write(make([]byte, 4)) // assume "checksum" is zero
	crc.Write(container.Payload)
	checksum := crc.Sum(nil)

	// Soter uses *reflected* Castagnoli CRC
	checksum[0], checksum[3] = checksum[3], checksum[0]
	checksum[1], checksum[2] = checksum[2], checksum[1]

	return checksum
}

// ParseSoterContainer extracts and validates a Soter container from the buffer.
// It also returns the remaining part of the buffer.
func ParseSoterContainer(buffer []byte) (*SoterContainer, []byte) {
	// Check that the buffer is at least big enough for a Soter container.
	if len(buffer) < 12 {
		return nil, buffer
	}

	// Read the header fields.
	var tagBytes [4]byte
	copy(tagBytes[:], buffer[0:4])
	length := binary.BigEndian.Uint32(buffer[4:8])
	checksum := buffer[8:12]

	// Check that we have enough remaining for the alleged payload.
	if len(buffer) < int(length) {
		return nil, buffer
	}
	payload := buffer[12:length]
	remaining := buffer[length:]

	// Verify the checksum.
	container := &SoterContainer{tagBytes, payload}
	if !bytes.Equal(checksum, container.Checksum()) {
		return nil, buffer
	}
	return container, remaining
}

// NewSoterContainer makes a new Soter container with given tag and payload.
//
// This is a convenience method so that you don't have to deal with arrays,
// as most of Themis tags are actually strings.
func NewSoterContainer(tag string, payload []byte) *SoterContainer {
	if len(tag) != 4 {
		panic("Soter container tag must be 4 bytes long")
	}
	var tagBytes [4]byte
	copy(tagBytes[:], tag)
	return &SoterContainer{tagBytes, payload}
}

// SerializeSoterContainer packs the tagged payload into a Soter container.
//
// This is a convenience method to quickly pack the payload into a byte slice.
func SerializeSoterContainer(tag string, payload []byte) []byte {
	return NewSoterContainer(tag, payload).Serialize(nil)
}

// Tag returns Soter container tag as a string.
func (container *SoterContainer) Tag() string {
	return string(container.TagBytes[:])
}
