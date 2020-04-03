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
    describe("secure message", function(){
	keypair = new addon.KeyPair();
	peer_keypair = new addon.KeyPair();
	intruder_keypair = new addon.KeyPair();
	encrypter = new addon.SecureMessage(keypair.private(), peer_keypair.public());
	decrypter = new addon.SecureMessage(peer_keypair.private(), keypair.public());
	intruder_decrypter = new addon.SecureMessage(intruder_keypair.private(), keypair.public());
	message = new Buffer("Test Message");
	it("encrypt/decrypt", function(){
	    encrypted_message = encrypter.encrypt(message);
	    assert.equal(message.toString(), decrypter.decrypt(encrypted_message).toString());
	    assert.throws(function(){intruder_decrypter.decrypt(encrypted_message);}, expect_code(addon.FAIL));
	});
	it("sign/verify", function(){
	    signed_message=encrypter.sign(message);
	    assert.equal(message.toString(), decrypter.verify(signed_message).toString());
	    assert.equal(message.toString(), intruder_decrypter.verify(signed_message).toString());
	    signed_message[2]++;
	    assert.throws(function(){decrypter.verify(signed_message);}, expect_code(addon.INVALID_PARAMETER));
	})
	it("empty keys", function(){
	    encrypted_message = encrypter.encrypt(message);
	    signed_message = encrypter.sign(message);

	    // check codes and messages when SM is empty
	    empty_secure_message = new addon.SecureMessage(new Buffer(""), new Buffer(""));
	    assert.throws(function(){empty_secure_message.encrypt(message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){empty_secure_message.encrypt(message);}, expect_message("private key is empty"));

	    assert.throws(function(){empty_secure_message.decrypt(encrypted_message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){empty_secure_message.decrypt(encrypted_message);}, expect_message("private key is empty"));

	    assert.throws(function(){empty_secure_message.sign(message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){empty_secure_message.sign(message);}, expect_message("private key is empty"));

	    assert.throws(function(){empty_secure_message.verify(signed_message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){empty_secure_message.verify(signed_message);}, expect_message("public key is empty"));

	    // check codes and messages when SM has no public key
	    secure_message_no_public = new addon.SecureMessage(keypair.private(), new Buffer(""));
	    assert.throws(function(){secure_message_no_public.encrypt(message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){secure_message_no_public.encrypt(message);}, expect_message("public key is empty"));

	    assert.throws(function(){secure_message_no_public.decrypt(encrypted_message);}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){secure_message_no_public.decrypt(encrypted_message);}, expect_message("public key is empty"));
	})
	it("sign/verify with single key", function() {
	    signer = new addon.SecureMessage(keypair.private(), new Buffer(""));
	    verifier = new addon.SecureMessage(new Buffer(""), keypair.public());
	    signed_message = signer.sign(message);
	    verified_message = verifier.verify(signed_message);
	    assert.equal(message.toString(), verified_message.toString());
	    assert.throws(function(){signer.verify(signed_message);}, expect_code(addon.INVALID_PARAMETER));
	})
	it("mismatched keys", function(){
	    assert.throws(function(){new addon.SecureMessage(keypair.public(), keypair.private())}, expect_code(addon.INVALID_PARAMETER));
	})
	it("keys misuse", function(){
	    assert.throws(function(){new addon.SecureMessage(new Buffer("i am not a real private key"), new Buffer(""))}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){new addon.SecureMessage(new Buffer(""), new Buffer("i am not a real public key"))}, expect_code(addon.INVALID_PARAMETER));
	})
    })
})

describe("jsthemis", function(){
    describe("secure session", function(){
	it("wrap/unwrap", function(){
	    message = new Buffer("This is test message");
	    server_id=new Buffer("server");
	    server_keypair = new addon.KeyPair();
	    client_id = new Buffer("client");
	    client_keypair = new addon.KeyPair();

	    server_session = new addon.SecureSession(server_id, server_keypair.private(), function(id){
		if(id.toString()=="server")
		    return server_keypair.public();
		else if(id.toString=="client".toString())
		    return client_keypair.public();
		return client_keypair.public();
	    });

	    client_session = new addon.SecureSession(client_id, client_keypair.private(), function(id){
		if(id.toString()=="server")
		    return server_keypair.public();
		else if(id.toString=="client")
		    return client_keypair.public();
	    });

	    data = client_session.connectRequest();
	    data = server_session.unwrap(data);
	    data = client_session.unwrap(data);
	    data = server_session.unwrap(data);
	    assert.equal(client_session.isEstablished(), false);
	    data = client_session.unwrap(data);
	    assert.equal(data,undefined);
	    assert.equal(server_session.isEstablished(), true);
	    assert.equal(client_session.isEstablished(), true);
	    data=client_session.wrap(message);
	    rm=server_session.unwrap(data);
	    assert.equal(message.toString(), rm.toString());

	    data=server_session.wrap(message);
	    rm=client_session.unwrap(data);
	    assert.equal(message.toString(), rm.toString());
	})
	it("invalid parameters", function(){
	    valid_id = new Buffer("client");
	    empty_id = new Buffer("");
	    keypair = new addon.KeyPair();
	    assert.throws(function(){new addon.SecureSession(empty_id, keypair.private(), function(){return null})},
	                  expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){new addon.SecureSession(valid_id, empty_id, function(){return null})},
	    	          expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){new addon.SecureSession(valid_id, keypair.private(), null)},
	    	          expect_code(addon.INVALID_PARAMETER));
	    server_session = new addon.SecureSession(valid_id, keypair.private(), function(){return null});
	    assert.throws(function(){server_session.unwrap(empty_id)}, expect_code(addon.INVALID_PARAMETER));
	})
        it("callback behavior", function(){
            message = new Buffer("This is test message")
            server_id = new Buffer("server")
            server_keypair = new addon.KeyPair()
            client_id = new Buffer("client")
            client_keypair = new addon.KeyPair()
            broken_id = new Buffer("broken")
            missing_id = new Buffer("missing")
            unknown_id = new Buffer("unknown")

            new_server_session = function() {
                return new addon.SecureSession(server_id, server_keypair.private(), function(id){
                    if(id.toString() == client_id.toString())
                        return client_keypair.public()
                    else if(id.toString() == missing_id.toString())
                        return null
                    else if(id.toString() == broken_id.toString())
                        return 42
                })
            }

            server_callback = function(id){
                if(id.toString() == server_id.toString())
                    return server_keypair.public()
            }
            client_sessions = []
            client_sessions.push(new addon.SecureSession(broken_id, client_keypair.private(), server_callback))
            client_sessions.push(new addon.SecureSession(missing_id, client_keypair.private(), server_callback))
            client_sessions.push(new addon.SecureSession(unknown_id, client_keypair.private(), server_callback))

            client_sessions.forEach(function(client_session){
                var server_session = new_server_session()
                var data

                data = client_session.connectRequest();
                assert.throws(function(){server_session.unwrap(data)}, expect_code(addon.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR));
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
        let emptyArray = new Uint8Array()
        let testInput = new Uint8Array([1, 1, 2, 3, 5, 8, 13])
        let testContext = new Uint8Array([42])
        describe('Seal mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => new addon.SecureCellSeal('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted)
                assert.deepEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('produces extended results', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                assert(encrypted.length > testInput.length)
            })
            it('forbits empty inputs', function() {
                assert.throws(() => new addon.SecureCellSeal(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = new addon.SecureCellSeal(masterKey1)
                assert.throws(() => cell.encrypt(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
            })
            it('empty context == no context', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                let decrypted = cell.decrypt(encrypted, emptyArray)
                assert.deepEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, null)
                let decrypted = cell.decrypt(encrypted)
                assert.deepEqual(decrypted, testInput)
            })
            it('detects invalid master key', function() {
                let cell1 = new addon.SecureCellSeal(masterKey1)
                let cell2 = new addon.SecureCellSeal(masterKey2)
                let encrypted = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('detects invalid context', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(encrypted, testInput),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                encrypted[20] = ~encrypted[20]
                assert.throws(() => cell.decrypt(encrypted),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = new addon.SecureCellSeal(masterKey1)
                let encrypted = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new addon.SecureCellSeal(invalid), TypeError)
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
                assert.throws(() => new addon.SecureCellTokenProtect('master key'), TypeError)
            })
            it('encrypts without context', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepEqual(decrypted, testInput)
            })
            it('encrypts with context', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(result.data, result.token, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('does not change encrypted data length', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                assert.equal(result.data.length, testInput.length)
                assert(result.token.length > 0)
            })
            it('forbids empty inputs', function() {
                assert.throws(() => new addon.SecureCellTokenProtect(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = new addon.SecureCellTokenProtect(masterKey1)
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
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, emptyArray)
                let decrypted = cell.decrypt(result.data, result.token)
                assert.deepEqual(decrypted, testInput)
            })
            it('null context == no context', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                let decrypted = cell.decrypt(result.data, result.token, null)
                assert.deepEqual(decrypted, testInput)
            })
            it('detects incorrect master key', function() {
                let cell1 = new addon.SecureCellTokenProtect(masterKey1)
                let cell2 = new addon.SecureCellTokenProtect(masterKey2)
                let result = cell1.encrypt(testInput)
                assert.throws(() => cell2.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('detects incorrect token', function() {
                let cell1 = new addon.SecureCellTokenProtect(masterKey1)
                let cell2 = new addon.SecureCellTokenProtect(masterKey2)
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
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput, testContext)
                assert.throws(() => cell.decrypt(result.data, result.token, testInput),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted data', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                result.data[5] = ~result.data[5]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('detects corrupted token', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                result.token[8] = ~result.token[8]
                assert.throws(() => cell.decrypt(result.data, result.token),
                    expect_code(addon.FAIL)
                )
            })
            it('handles type mismatches', function() {
                let cell = new addon.SecureCellTokenProtect(masterKey1)
                let result = cell.encrypt(testInput)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new addon.SecureCellTokenProtect(invalid), TypeError)
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
        })
        describe('Context Imprint mode', function() {
            it('does not accept strings', function() {
                assert.throws(() => new addon.SecureCellContextImprint('master key'), TypeError)
            })
            it('encrypts only with context', function() {
                let cell = new addon.SecureCellContextImprint(masterKey1)
                assert.throws(() => cell.encrypt(testInput))
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.deepEqual(decrypted, testInput)
            })
            it('encryption does not change data length', function() {
                let cell = new addon.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                assert.equal(encrypted.length, testInput.length)
            })
            it('forbids empty message and context', function() {
                assert.throws(() => new addon.SecureCellContextImprint(emptyArray),
                    expect_code(addon.INVALID_PARAMETER)
                )
                let cell = new addon.SecureCellContextImprint(masterKey1)
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
                let cell1 = new addon.SecureCellContextImprint(masterKey1)
                let cell2 = new addon.SecureCellContextImprint(masterKey2)
                let encrypted = cell1.encrypt(testInput, testContext)
                let decrypted = cell2.decrypt(encrypted, testContext)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('does not detect incorrect context', function() {
                let cell = new addon.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                let decrypted = cell.decrypt(encrypted, testInput)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('does not detect corrupted data', function() {
                let cell = new addon.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                encrypted[5] = ~encrypted[5]
                let decrypted = cell.decrypt(encrypted, testContext)
                assert.notDeepEqual(testInput, decrypted)
            })
            it('handles type mismatches', function() {
                let cell = new addon.SecureCellContextImprint(masterKey1)
                let encrypted = cell.encrypt(testInput, testContext)
                generallyInvalidArguments.forEach(function(invalid) {
                    assert.throws(() => new addon.SecureCellContextImprint(invalid), TypeError)
                    assert.throws(() => cell.encrypt(invalid, testContext), TypeError)
                    assert.throws(() => cell.decrypt(invalid, testContext), TypeError)
                    assert.throws(() => cell.encrypt(testInput, invalid), TypeError)
                    assert.throws(() => cell.decrypt(encrypted, invalid), TypeError)
                })
            })
        })
    })
})

describe("jsthemis", function(){
    describe("secure comparator", function(){
	it("match", function(){
	    server_secret = new Buffer("Secret");
	    client_secret = new Buffer("Secret");
	    server_comparator = new addon.SecureComparator(server_secret);
	    client_comparator = new addon.SecureComparator(client_secret);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = client_comparator.beginCompare();
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = server_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = client_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = server_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), true);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.equal(server_comparator.isMatch(), true);
	    data = client_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), true);
	    assert.equal(server_comparator.isCompareComplete(), true);
	    assert.equal(client_comparator.isMatch(), true);
	    assert.equal(server_comparator.isMatch(), true);
	});
	it("not match", function(){
	    server_secret = new Buffer("Secret1");
	    client_secret = new Buffer("Secret2");
	    server_comparator = new addon.SecureComparator(server_secret);
	    client_comparator = new addon.SecureComparator(client_secret);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = client_comparator.beginCompare();
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = server_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = client_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), false);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.throws(function(){server_comparator.isMatch()})
	    data = server_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), false);
	    assert.equal(server_comparator.isCompareComplete(), true);
	    assert.throws(function(){client_comparator.isMatch()})
	    assert.equal(server_comparator.isMatch(), false);
	    data = client_comparator.proceedCompare(data);
	    assert.equal(client_comparator.isCompareComplete(), true);
	    assert.equal(server_comparator.isCompareComplete(), true);
	    assert.equal(client_comparator.isMatch(), false);
	    assert.equal(server_comparator.isMatch(), false);
	})
        it("invalid parameters", function() {
            let empty = Buffer.from('')
            assert.throws(() => new addon.SecureComparator(empty),
                expect_code(addon.INVALID_PARAMETER))
            server_comparator = new addon.SecureComparator(server_secret)
            assert.throws(() => server_comparator.proceedCompare(empty),
                expect_code(addon.INVALID_PARAMETER))
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
