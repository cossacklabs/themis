// Copyright (c) 2016 Cossack Labs Limited
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

var addon = require('..');
var assert = require('assert');

function expect_code(code) {
  return function(err) {
    return err.code == code
  }
}

function expect_message(message) {
  return function(err) {
    return err.message.includes(message)
  }
}

let generallyInvalidArguments = [
	null, undefined, 'string',
	new Int16Array([1, 2, 3]), [4, 5, 6],
	() => new Uint8Array([27, 18, 28, 18, 28]),
	{ value: [3, 14, 15, 92, 6] }
]

describe("jsthemis", function(){
    describe('Secure Message', function() {
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        let keyPairA = new addon.KeyPair()
        let keyPairB = new addon.KeyPair()
        let privateKeyA = keyPairA.private()
        let privateKeyB = keyPairB.private()
        let publicKeyA = keyPairA.public()
        let publicKeyB = keyPairB.public()
        describe('encrypt/decrypt mode', function() {
            it('requires valid keys', function() {
                let keyPair = new addon.KeyPair()
                let privateKey = keyPair.private()
                let publicKey = keyPair.public()
                // You should provide a private and public key
                new addon.SecureMessage(privateKey, publicKey)
                // But they must be private *and* public (in this order)
                assert.throws(() => new addon.SecureMessage(publicKey, publicKey))
                assert.throws(() => new addon.SecureMessage(publicKey, privateKey))
                assert.throws(() => new addon.SecureMessage(privateKey, privateKey))
                // Both must be specified, you can't skip arguments
                assert.throws(() => new addon.SecureMessage(privateKey))
                assert.throws(() => new addon.SecureMessage(publicKey))
                // And these really need to be keys
                assert.throws(() => new addon.SecureMessage(privateKey, Buffer.from('nope')))
                assert.throws(() => new addon.SecureMessage(Buffer.from('not a key'), publicKey))
            })
            it('encrypts and decrypts', function() {
                let secureMessageAlice = new addon.SecureMessage(privateKeyA, publicKeyB)
                let secureMessageBob = new addon.SecureMessage(privateKeyB, publicKeyA)

                let encrypted = secureMessageAlice.encrypt(testInput)
                let decrypted = secureMessageBob.decrypt(encrypted)

                assert.deepEqual(testInput, decrypted)
            })
            it('does not allow empty messages', function() {
                let secureMessage = new addon.SecureMessage(privateKeyA, publicKeyB)

                assert.throws(() => secureMessage.encrypt(emptyArray))
            })
            it('cannot decrypt with a different key', function() {
                let secureMessageAlice = new addon.SecureMessage(privateKeyA, publicKeyB)
                let privateKeyE = new addon.KeyPair().private()
                let secureMessageEveA = new addon.SecureMessage(privateKeyE, publicKeyA)
                let secureMessageEveB = new addon.SecureMessage(privateKeyE, publicKeyB)

                let encrypted = secureMessageAlice.encrypt(testInput)

                assert.throws(() => secureMessageEveA.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
                assert.throws(() => secureMessageEveB.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let secureMessageAlice = new addon.SecureMessage(privateKeyA, publicKeyB)
                let secureMessageBob = new addon.SecureMessage(privateKeyB, publicKeyA)

                let encrypted = secureMessageAlice.encrypt(testInput)
                encrypted[10] = ~encrypted[10]

                assert.throws(() => secureMessageBob.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let secureMessage = new addon.SecureMessage(privateKeyA, publicKeyB)
                generallyInvalidArguments.forEach(function(invalid) {
                    // null values are okay (sign/verify mode)
                    if (invalid !== null) {
                        assert.throws(() => new addon.SecureMessage(privateKeyA, invalid), TypeError)
                        assert.throws(() => new addon.SecureMessage(invalid, publicKeyB), TypeError)
                    }
                    assert.throws(() => secureMessage.encrypt(invalid), TypeError)
                    assert.throws(() => secureMessage.decrypt(invalid), TypeError)
                })
            })
        })
        describe('sign/verify mode', function() {
            it('requires valid keys', function() {
                let keyPair = new addon.KeyPair()
                let privateKey = keyPair.private()
                let publicKey = keyPair.public()
                // You can omit either private or public key, but not both
                new addon.SecureMessage(null, publicKey)
                new addon.SecureMessage(privateKey, null)
                assert.throws(() => new addon.SecureMessage(null, null))
                assert.throws(() => new addon.SecureMessage(null))
                // Empty buffers are okay too
                new addon.SecureMessage(Buffer.from(''), publicKey)
                new addon.SecureMessage(privateKey, Buffer.from(''))
                // But they need to be keys
                assert.throws(() => new addon.SecureMessage(null, Buffer.from('nope')))
                assert.throws(() => new addon.SecureMessage(Buffer.from('not a key'), null))
            })
            it('signs and verifies', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyA)

                let signed = signer.sign(testInput)
                let verified = verifier.verify(signed)

                assert.deepEqual(testInput, verified)
            })
            it('does not allow empty messages', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyA)

                assert.throws(() => signer.sign(emptyArray))
                assert.throws(() => verifier.verify(emptyArray))
            })
            it('leaves signed data in plaintext', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)

                let signed = signer.sign(testInput)

                assert.ok(signed.includes(testInput))
            })
            it('cannot verify with a different key', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyB)

                let signed = signer.sign(testInput)

                assert.throws(() => verifier.verify(signed),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyA)

                let signed = signer.sign(testInput)
                signed[12] = ~signed[12]

                assert.throws(() => verifier.verify(signed),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyA)
                generallyInvalidArguments.forEach(function(invalid) {
                    // Null keys are handled specially
                    if (invalid !== null) {
                        assert.throws(() => new addon.SecureMessage(invalid, null), TypeError)
                        assert.throws(() => new addon.SecureMessage(null, invalid), TypeError)
                    }
                    assert.throws(() => signer.sign(invalid), TypeError)
                    assert.throws(() => verifier.verify(invalid), TypeError)
                })
            })
            it('can sign with keys present', function() {
                // ...though the public key does not matter for signatures,
                // and private key is ignored for verification.
                let keyPairE = new addon.KeyPair()
                let signer = new addon.SecureMessage(privateKeyA, keyPairE.public())
                let verifier = new addon.SecureMessage(privateKeyB, publicKeyA)

                let signed = signer.sign(testInput)
                let verified = verifier.verify(signed)

                assert.deepEqual(testInput, verified)
            })
            it('cannot be used for encryption', function() {
                let signer = new addon.SecureMessage(privateKeyA, null)
                let verifier = new addon.SecureMessage(null, publicKeyA)
                let encryptor = new addon.SecureMessage(privateKeyA, publicKeyA)

                let encrypted = encryptor.encrypt(testInput)

                // You need both keys to encrypt or decrypt data.
                assert.throws(() => signer.encrypt(testInput), expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => signer.decrypt(encrypted), expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => verifier.encrypt(testInput), expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => verifier.decrypt(encrypted), expect_code(addon.INVALID_PARAMETER))
            })
            it('is not compatible with encryption', function() {
                let encryptor = new addon.SecureMessage(privateKeyA, publicKeyA)

                let encrypted = encryptor.encrypt(testInput)
                let signed = encryptor.sign(testInput)

                // Encrypted data cannot be verified, and signed data cannot be decrypted
                assert.throws(() => encryptor.verify(encrypted), expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => encryptor.decrypt(signed), expect_code(addon.INVALID_PARAMETER))
            })
        })
    })
})

describe("jsthemis", function(){
    describe('Secure Session', function() {
        let emptyArray = Buffer.from('')
        let randomID = Buffer.from('random')
        let clientID = Buffer.from('client')
        let serverID = Buffer.from('server')
        let messageA = Buffer.from('testing, testing')
        let messageB = Buffer.from('everything seems to be in order')
        // JS API does not provide methods for generating RSA keys,
        // we have to import them from buffers instead
        let rsaKeyPair = addon.KeyPair(Buffer.from("UlJBMgAABJBm8NGvTdCr5R7/iNvQy+WrC6DsYPg5ze5dc1/UxsthqEE4t+4Euwq6IVdddjisxUBvzWQ0VuyQbaExbCrrIaMYHgF9DSNOyrCznrHKBlNZrfxM+pHIkVB6q+b2hLEX4j3wnMZKE0dE0W3SGO/p0oYCsF0gjC7kNTjsh67WwjsIo6EqOrAG7RF9p3UaGUNm4tGEKCAM4NE8d8URcaWSIhQIHeOrVVbziWdMFiOX2GRGwpi2lWFxTLhI1Tyyov+NLy/J5b7NL8n7qusC13eG2XPMrEucuhZtihczn5l4wGkrPi/+jAW+Kn09rQERAPe1rOXHcpu2PzVOdO0bBkqK6IzVylgxQepMriLBQzaEZwQ3BER0byG/k6H1wcvdt9qo+6byB/jHs3hnbqJP4H9HdrJQJshxko8HqS2UHxhSFUaW2H7LR2vD68YBMnUf+JP/79D68sVWXj8/s68PqXw/iLjZcLY+94adKa8FmqYB/MD0tYXGc4+G+ZmPYZGUF1apkQOIvdI5y36+d+UmxdpGHH4KR+JcOsyIzDDTlWKbFk3sIqZzg18cFEVgK6ujc4lCOAPCbhiDic1GKeYxNDEShhI54l+b3FV2Fxy7nPVqRBBumx/Zp56qxHK32Fm2O8WuslReRxXQbOxslfyeNlnGIXMLWBXCw+eBw90pEhaJ0G//a5MXfyVF50TZB7Im9M/LeSKjzd1OQ1snqiv8yBTPfzU9tN5eAi7wkGN4t74yGz81qiY4DXJ9i/qUp6mzVuIUInNLORkA9P5GIBtOyJAgj+6b+hMufzs1Qas5YsHdWRNQm0c+iIDPP9qs5k4ik1s2sovp8rCNtOaJQzcH+RgRehvS4xxHOQuEP8lPSbN21/Ly2ygYUnnta2yK8t6Idxh7DnsjinQL5O3swzWVgCmgwgOjitRqRL9+w5LJeU1n3ZGC5I3ANXTxzT4fnSqT5RC3EVtsIQhQfX+ism7nHN4jIPDsv20seeBAPSD77ARNbl4kQacZjC1EI97u3E0uw1vtiroO2AyVIO937I7l1IR4u9SIj6imKiTrfDkie/Yi98Z9OESQJtb48Zg5IaDSccs33vbY1cE4SNVgh+b1oQTNUGL7TFCPfh2Y0i4VFnvrwQ9EDoP6UqreP/CKaI3IezvmPH3cDyS9TsCzVnwSBm+2h5dIFjZbIE3EiaM2Rtzvz2XvH7TpZ2e6Ps4l6Pg9I0MRiyABTwA5L3a8hN169D13sKXtJDpZjbixtGUCbRNK+18/wRIsWkrJF1kn5hzO63s1hFhXcuSYDxH85C3+EYu7/O76bXz9A+/yxXSDbYB5PKTzgf/WoPuLtVtZKPa7kFvekM/WHSQBtwrdZlpRqzTx+WhX5kA/FC45ADdVO4jpGJH7+9JOHsjWg1u9/7qnssTCFXz8zB+8Gf8AHfgMdARt2KN81j7EgBrL7+YC5rUu5vR+tdUheGkDjbAP6Zl266UB+iTTxBO08qXMYV9/fFSn6SAK7crwaSInntXBYgjl/JM0WKnh2e0kIgCITfswK2a9o/CM2Kk9AAEAAQ==", "base64"), Buffer.from("VVJBMgAAARB1/lOFuj7OJej4PSNDEYsgAU8AOS92vITdevQ9d7Cl7SQ6WY24sbRlAm0TSvtfP8ESLFpKyRdZJ+Yczut7NYRYV3LkmA8R/OQt/hGLu/zu+m18/QPv8sV0g22AeTyk84H/1qD7i7VbWSj2u5Bb3pDP1h0kAbcK3WZaUas08floV+ZAPxQuOQA3VTuI6RiR+/vSTh7I1oNbvf+6p7LEwhV8/MwfvBn/AB34DHQEbdijfNY+xIAay+/mAua1Lub0frXVIXhpA42wD+mZduulAfok08QTtPKlzGFff3xUp+kgCu3K8GkiJ57VwWII5fyTNFip4dntJCIAiE37MCtmvaPwjNipPQABAAE=", "base64"))

        function makeKeyMaterial() {
            let keys = {}
            keys.client = new addon.KeyPair()
            keys.server = new addon.KeyPair()
            keys.clientCallback = function(id) {
                if (id.equals(serverID)) {
                    return keys.server.public()
                }
                return null
            }
            keys.serverCallback = function(id) {
                if (id.equals(clientID)) {
                    return keys.client.public()
                }
                return null
            }
            return keys
        }

        it('establishes communication', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(serverID, keys.server.private(), keys.serverCallback)

            let data = client.connectRequest()
            let peer = server
            while (!(client.isEstablished() && server.isEstablished())) {
                data = peer.unwrap(data)
                if (peer == server) {
                    peer = client
                } else {
                    peer = server
                }
            }

            let wrappedMessageA = client.wrap(messageA)
            let unwrappedMessageA = server.unwrap(wrappedMessageA)
            assert.deepEqual(unwrappedMessageA, messageA)

            let wrappedMessageB = server.wrap(messageB)
            let unwrappedMessageB = client.unwrap(wrappedMessageB)
            assert.deepEqual(unwrappedMessageB, messageB)
        })
        it('handles unknown clients', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(randomID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(serverID, keys.server.private(), keys.serverCallback)

            let request = client.connectRequest()
            assert.throws(() => server.unwrap(request),
                expect_code(addon.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR)
            )
        })
        it('handles unknown servers', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(randomID, keys.server.private(), keys.serverCallback)

            let request = client.connectRequest()
            let reply = server.unwrap(request)
            assert.throws(() => client.unwrap(reply),
                expect_code(addon.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR)
            )
        })
        it('does not allow empty client ID', function() {
            let keyPair = new addon.KeyPair()
            assert.throws(() => new addon.SecureSession(emptyArray, keyPair.private(), function(){}),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('allows EC private key', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), keys.clientCallback)
            assert.ok(client)
        })
        it('does not allow EC public key', function() {
            let keyPair = new addon.KeyPair()
            assert.throws(() => new addon.SecureSession(clientID, keyPair.public(), function(){}),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('does not allow RSA key', function() {
            assert.throws(() => new addon.SecureSession(clientID, rsaKeyPair.private(), function(){}),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('does not allow RSA public key', function() {
            assert.throws(() => new addon.SecureSession(clientID, rsaKeyPair.public(), function(){}),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('does not allow empty message', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(serverID, keys.server.private(), keys.serverCallback)

            let data = client.connectRequest()
            let peer = server
            while (!(client.isEstablished() && server.isEstablished())) {
                data = peer.unwrap(data)
                if (peer == server) {
                    peer = client
                } else {
                    peer = server
                }
            }

            assert.throws(() => server.wrap(emptyArray),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('does not allow public keys in constructor', function() {
            let keyPair = new addon.KeyPair()
            assert.throws(() => new addon.SecureSession(clientID, keyPair.public(), function(){}),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('does not allow private keys in callback', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), function(id) {
                if (id.equals(serverID)) {
                    return keys.server.private()
                }
            })
            let server = new addon.SecureSession(serverID, keys.server.private(), keys.serverCallback)

            let request = client.connectRequest()
            let reply = server.unwrap(request)
            assert.throws(() => client.unwrap(reply),
                expect_code(addon.INVALID_PARAMETER)
            )
        })
        it('handles exception in callback', function() {
            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(randomID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(serverID, keys.server.private(), function(id) {
                throw Error('something')
            })

            let request = client.connectRequest()
            assert.throws(() => server.unwrap(request),
                expect_code(addon.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR)
            )
        })
        it('handles type mismatches', function() {
            function isFunction(obj) {
                return !!(obj && obj.constructor && obj.call && obj.apply)
            }

            let keys = makeKeyMaterial()
            let client = new addon.SecureSession(clientID, keys.client.private(), keys.clientCallback)
            let server = new addon.SecureSession(serverID, keys.server.private(), keys.serverCallback)

            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(
                    () => new addon.SecureSession(invalid, keys.client.private(), keys.clientCallback),
                    TypeError
                )
                assert.throws(
                    () => new addon.SecureSession(clientID, invalid, keys.clientCallback),
                    TypeError
                )
                // Functions are actually okay for callbacks
                if (!isFunction(invalid)) {
                    assert.throws(
                        () => new addon.SecureSession(clientID, keys.client.private(), invalid),
                        expect_code(addon.INVALID_PARAMETER)
                    )
                }

                assert.throws(() => client.unwrap(invalid), TypeError)
                assert.throws(() => server.unwrap(invalid), TypeError)
            })

            let data = client.connectRequest()
            let peer = server
            while (!(client.isEstablished() && server.isEstablished())) {
                data = peer.unwrap(data)
                if (peer == server) {
                    peer = client
                } else {
                    peer = server
                }
            }

            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => client.wrap(invalid), TypeError)
                assert.throws(() => client.unwrap(invalid), TypeError)
                assert.throws(() => server.wrap(invalid), TypeError)
                assert.throws(() => server.unwrap(invalid), TypeError)
            })
        })
    })
})

describe("jsthemis", function(){
    describe('Secure Cell', function() {
        describe('key generation', function() {
            const defaultLength = 32
            it('generates new keys', function() {
                let key = new addon.SymmetricKey()
                assert.equal(key.length, defaultLength)
            })
            it("generates new instances", function(){
                // Check that we don't reuse the same object
                var key1 = new addon.SymmetricKey()
                var key2 = new addon.SymmetricKey()
                assert.notDeepEqual(key1, key2)
                assert.notEqual(key1, key2)
                // A copy should have the same content,
                // but it's a distint object
                var key3 = new addon.SymmetricKey(key2)
                assert.deepEqual(key3, key2)
                assert.notEqual(key3, key2)
                assert.notDeepEqual(key3, key1)
                assert.notEqual(key3, key1)
            })
            it('wraps existing keys', function() {
                let buffer = Buffer.from("MDRwUzB0NG1aN2pvTEEwdVljRFJ5", "base64")
                let key = new addon.SymmetricKey(buffer)
                assert.deepEqual(key, buffer)
            })
            it('fails with empty buffer', function() {
                assert.throws(() => new addon.SymmetricKey(Buffer.from("")),
                    expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => new addon.SymmetricKey(new Uint8Array()),
                    expect_code(addon.INVALID_PARAMETER))
                assert.throws(() => new addon.SymmetricKey(""), TypeError)
                assert.throws(() => new addon.SymmetricKey(null), TypeError)
                assert.throws(() => new addon.SymmetricKey(undefined), TypeError)
            })
            it('fails with invalid types', function() {
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new addon.SymmetricKey(invalid),
                        TypeError
                    )
                })
            })
        })
        let masterKey1 = new Uint8Array([1, 2, 3, 4])
        let masterKey2 = new Uint8Array([5, 6, 7, 8, 9])
        let passphrase1 = "I don't always use a passphrase..."
        let passphrase2 = "but when I do, I make sure it's not a meme"
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        let testContext = new Uint8Array([42])
        describe('Seal mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => addon.SecureCellSeal.withKey('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                assert(encrypted.length > testInput.length)
            })
            it('forbits empty inputs', function() {
                assert.throws(() => addon.SecureCellSeal.withKey(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)
                assert.deepEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)
                assert.deepEqual(decrypted, testInput)
            })
            it('detects invalid master key', function() {
                let cell1 = addon.SecureCellSeal.withKey(masterKey1)
                let cell2 = addon.SecureCellSeal.withKey(masterKey2)
                let encrypted = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                encrypted[20] = ~encrypted[20]
                assert.throws(() => cell.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = addon.SecureCellSeal.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => addon.SecureCellSeal.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid), TypeError)
                    // null context is okay, it should not throw
                    if (invalid !== null) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                    }
                })
            })
            it('can be constructed with "new"', function() {
                // Pre-0.13 syntax:
                let cellOld = new addon.SecureCellSeal(masterKey1)
                let cellNew = addon.SecureCellSeal.withKey(masterKey1)

                let encrypted = cellOld.encrypt(testInput)
                let decrypted = cellNew.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
            it('can be constructed with bare constructor', function() {
                // Historically allowed compatibility syntax:
                let cellOld = addon.SecureCellSeal(masterKey1)
                let cellNew = addon.SecureCellSeal.withKey(masterKey1)

                let encrypted = cellOld.encrypt(testInput)
                let decrypted = cellNew.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
        })
        describe('Seal mode (passphrase)', function() {
            // Passphrase API uses KDF so it can be quite slow.
            // Mocha uses default threshold of 75 ms which is not enough.
            this.slow(400) // milliseconds

            it('encrypts without context', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)

                assert.deepEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput)

                assert(encrypted.length > testInput.length)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => addon.SecureCellSeal.withPassphrase(''),
                    expect_code(addon.INVALID_PARAMETER)
                )
                assert.throws(() => addon.SecureCellSeal.withPassphrase(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)

                assert.deepEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
            it('detects invalid passphrase', function() {
                let cell1 = addon.SecureCellSeal.withPassphrase(passphrase1)
                let cell2 = addon.SecureCellSeal.withPassphrase(passphrase2)

                let encrypted = cell1.encrypt(testInput)

                assert.throws(() => cell2.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput, testContext)

                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)

                let encrypted = cell.encrypt(testInput)
                encrypted[20] = ~encrypted[20]

                assert.throws(() => cell.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('accepts byte arrays', function() {
                let cell = addon.SecureCellSeal.withPassphrase(masterKey1)

                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
            it('passphrase is not master key', function() {
                let cellMK = addon.SecureCellSeal.withKey(masterKey1)
                let cellPW = addon.SecureCellSeal.withPassphrase(masterKey1)

                let encryptedMK = cellMK.encrypt(testInput)

                assert.throws(() => cellPW.decrypt(encryptedMK),
                    expect_code(addon.FAIL)
                )
                let encryptedPW = cellPW.encrypt(testInput)
                assert.throws(() => cellMK.decrypt(encryptedPW),
                    expect_code(addon.FAIL)
                )
            })
            it('encodes passphrase in UTF-8', function() {
                let passphrase = 'нікому не кажи цей пароль'
                let cell1 = addon.SecureCellSeal.withPassphrase(passphrase)
                let cell2 = addon.SecureCellSeal.withPassphrase(Buffer.from(passphrase, 'UTF-8'))

                let encrypted = cell1.encrypt(testInput)
                let decrypted = cell2.decrypt(encrypted)

                assert.deepEqual(decrypted, testInput)
            })
            it('handles type mismatches', function() {
                let cell = addon.SecureCellSeal.withPassphrase(passphrase1)
                let encrypted = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    // strings are okay (only) for passphrase API
                    if (typeof invalid !== 'string') {
                        assert.throws(() => addon.SecureCellSeal.withPassphrase(invalid), TypeError)
                    }
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid), TypeError)
                    // null context is okay, it should not throw
                    if (invalid !== null) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                    }
                })
            })
        })
        describe('Token Protect mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => addon.SecureCellTokenProtect.withKey('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(result.data, result.token, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('does not change encrypted data length', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                assert.equal(result.data.length, testInput.length)
                assert(result.token.length > 0)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => addon.SecureCellTokenProtect.withKey(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let result = cell.encrypt(testInput)
                assert.throws(() => cell.decrypt(emptyArray, result.token),
                    expect_code(addon.INVALID_PARAMETER)
                )
                assert.throws(() => cell.decrypt(result.data, emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, emptyArray)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token, null)
                assert.deepEqual(decrypted, testInput)
            })
            it('detects incorrect master key', function() {
                let cell1 = addon.SecureCellTokenProtect.withKey(masterKey1)
                let cell2 = addon.SecureCellTokenProtect.withKey(masterKey2)
                let result = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('detects incorrect token', function() {
                let cell1 = addon.SecureCellTokenProtect.withKey(masterKey1)
                let cell2 = addon.SecureCellTokenProtect.withKey(masterKey2)
                let result1 = cell1.encrypt(testInput)
                let result2 = cell2.encrypt(testInput)
                assert.throws(() => cell1.decrypt(result1.data, result2.token),
                    expect_code(addon.FAIL)
                )
                assert.throws(() => cell2.decrypt(result2.data, result1.token),
                    expect_code(addon.FAIL)
                )
            })
            it('detects incorrect context', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(result.data, result.token, testInput),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                result.data[5] = ~result.data[5]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted token', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                result.token[8] = ~result.token[8]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = addon.SecureCellTokenProtect.withKey(masterKey1)
                let result = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => addon.SecureCellTokenProtect.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid), TypeError)
                    assert.throws(() => cell.decrypt(result.data, invalid), TypeError)
                    assert.throws(() => cell.decrypt(invalid, result.token), TypeError)
                    // null context is okay, it should not throw
                    if (invalid !== null) {
                        assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                        assert.throws(() => cell.decrypt(result.data, result.token, invalid), TypeError)
                    }
                })
            })
            it('can be constructed with "new"', function() {
                // Pre-0.13 syntax:
                let cellOld = new addon.SecureCellTokenProtect(masterKey1)
                let cellNew = addon.SecureCellTokenProtect.withKey(masterKey1)

                let result = cellOld.encrypt(testInput)
                let decrypted = cellNew.decrypt(result.data, result.token)

                assert.deepEqual(decrypted, testInput)
            })
            it('can be constructed with bare constructor', function() {
                // Historically allowed compatibility syntax:
                let cellOld = addon.SecureCellTokenProtect(masterKey1)
                let cellNew = addon.SecureCellTokenProtect.withKey(masterKey1)

                let result = cellOld.encrypt(testInput)
                let decrypted = cellNew.decrypt(result.data, result.token)

                assert.deepEqual(decrypted, testInput)
            })
        })
        describe('Context Imprint mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => addon.SecureCellContextImprint.withKey('master key'), TypeError)
            })
            it('encrypts only with context', function() {
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                assert.throws(() => cell.encrypt(testInput))
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('encryption does not change data length', function() {
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.equal(encrypted.length, testInput.length)
            })
            it('forbids empty message and context', function() {
                assert.throws(() => addon.SecureCellContextImprint.withKey(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray, testContext))
                assert.throws(() => cell.encrypt(null,       testContext))
                assert.throws(() => cell.encrypt(testInput,  emptyArray))
                assert.throws(() => cell.encrypt(testInput,  null))
                assert.throws(() => cell.encrypt(emptyArray, emptyArray))
                assert.throws(() => cell.encrypt(null,       null))
                assert.throws(() => cell.decrypt(emptyArray, testContext))
                assert.throws(() => cell.decrypt(null,       testContext))
                assert.throws(() => cell.decrypt(testInput,  emptyArray))
                assert.throws(() => cell.decrypt(testInput,  null))
                assert.throws(() => cell.decrypt(emptyArray, emptyArray))
                assert.throws(() => cell.decrypt(null,       null))
            })
            it('does not detect incorrect master key', function() {
                let cell1 = addon.SecureCellContextImprint.withKey(masterKey1)
                let cell2 = addon.SecureCellContextImprint.withKey(masterKey2)
                let encrypted = cell1.encrypt(testInput, testContext)
                let decrypted = cell2.decrypt(encrypted, testContext)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('does not detect incorrect context', function() {
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testInput)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('does not detect corrupted data', function() {
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                encrypted[5] = ~encrypted[5]
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('handles type mismatches', function() {
                let cell = addon.SecureCellContextImprint.withKey(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => addon.SecureCellContextImprint.withKey(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid, testContext), TypeError)
                    assert.throws(() => cell.decrypt(invalid, testContext), TypeError)
                    assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                    assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                })
            })
            it('can be constructed with "new"', function() {
                // Pre-0.13 syntax:
                let cellOld = new addon.SecureCellContextImprint(masterKey1)
                let cellNew = addon.SecureCellContextImprint.withKey(masterKey1)

                let encrypted = cellOld.encrypt(testInput, testContext)
                let decrypted = cellNew.decrypt(encrypted, testContext)

                assert.deepEqual(decrypted, testInput)
            })
            it('can be constructed with bare constructor', function() {
                // Historically allowed compatibility syntax:
                let cellOld = addon.SecureCellContextImprint(masterKey1)
                let cellNew = addon.SecureCellContextImprint.withKey(masterKey1)

                let encrypted = cellOld.encrypt(testInput, testContext)
                let decrypted = cellNew.decrypt(encrypted, testContext)

                assert.deepEqual(decrypted, testInput)
            })

        })
    })
})

describe("jsthemis", function(){
    describe('Secure Comparator', function() {
        let secretBytes = Buffer.from('secret')
        let randomBytes = Buffer.from('random')
        it('confirms matching data', function() {
            let comparison1 = new addon.SecureComparator(secretBytes)
            let comparison2 = new addon.SecureComparator(secretBytes)

            let data = comparison1.beginCompare()
            while (!comparison1.isCompareComplete() && !comparison2.isCompareComplete()) {
                data = comparison2.proceedCompare(data)
                data = comparison1.proceedCompare(data)
            }

            assert(comparison1.isMatch())
            assert(comparison2.isMatch())
        })
        it('notices different data', function() {
            let comparison1 = new addon.SecureComparator(secretBytes)
            let comparison2 = new addon.SecureComparator(randomBytes)

            let data = comparison1.beginCompare()
            while (!comparison1.isCompareComplete() && !comparison2.isCompareComplete()) {
                data = comparison2.proceedCompare(data)
                data = comparison1.proceedCompare(data)
            }

            assert(!comparison1.isMatch())
            assert(!comparison2.isMatch())
        })
        it('handles simultaneous start', function() {
            let comparison1 = new addon.SecureComparator(secretBytes)
            let comparison2 = new addon.SecureComparator(secretBytes)

            let data1 = comparison1.beginCompare()
            let data2 = comparison2.beginCompare()

            assert.throws(() => comparison1.proceedCompare(data2))
            assert.throws(() => comparison2.proceedCompare(data1))
            assert(!comparison1.isCompareComplete())
            assert(!comparison2.isCompareComplete())
        })
        it('does not allow reusing', function() {
            let comparison1 = new addon.SecureComparator(secretBytes)
            let comparison2 = new addon.SecureComparator(secretBytes)

            let data = comparison1.beginCompare()
            while (!comparison1.isCompareComplete() && !comparison2.isCompareComplete()) {
                data = comparison2.proceedCompare(data)
                data = comparison1.proceedCompare(data)
            }

            // Cannot restart comparison again after starting it
            assert.throws(() => comparison1.beginCompare())
        })
        it('does not allow strings', function() {
            // Technically, it is possible to allow strings (e.g., decode them as UTF-8)
            // but for consistency with other wrappers we allow only byte arrays for now.
            assert.throws(() => new addon.SecureComparator('secret'))
        })
        it('does not allow empty data', function() {
            assert.throws(() => new addon.SecureComparator())
            assert.throws(() => new addon.SecureComparator(Buffer.from('')))
            let comparison = new addon.SecureComparator(secretBytes)
            assert.throws(() => comparison.proceedCompare(Buffer.from('')))
        })
        it('handles type mismatches', function() {
            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() =>  new addon.SecureComparator(invalid), TypeError)
            })
            let comparison = new addon.SecureComparator(secretBytes)
            generallyInvalidArguments.forEach(function(invalid) {
                assert.throws(() => comparison.proceedCompare(invalid), TypeError)
            })
        })
    })
})

describe("jsthemis", function(){
    describe("key generation", function(){
        let empty_message = Buffer.from('')
        it("generates and joins", function(){
            pair = new addon.KeyPair()
            new_pair = new addon.KeyPair(pair.private(), pair.public())
            assert.deepEqual(pair.private(), new_pair.private())
            assert.deepEqual(pair.public(), new_pair.public())
            assert.throws(function(){new addon.KeyPair(empty_message, empty_message)}, expect_code(addon.INVALID_PARAMETER));
        })
    })
})
