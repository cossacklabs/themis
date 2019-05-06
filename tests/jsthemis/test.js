var addon = require('jsthemis');
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
    describe("secure cell", function(){
	message=new Buffer("This is test message");
	password=new Buffer("This is test password");
	context=new Buffer("This is test context");
	empty_message=new Buffer("");
	it("seal (with context)", function(){
	    seal_encrypter = new addon.SecureCellSeal(password);
	    seal_decrypter = new addon.SecureCellSeal(password);
	    seal_intruder_decrypter = new addon.SecureCellSeal(new Buffer("This is test password1"));
	    assert.throws(function(){new addon.SecureCellSeal(empty_message)}, expect_code(addon.INVALID_PARAMETER));
	    enc_data = seal_encrypter.encrypt(message, context);
	    dec_data = seal_decrypter.decrypt(enc_data, context);
	    assert.equal(message.toString(), dec_data.toString());
	    assert.throws(function(){seal_intruder_decrypter.decrypt(enc_data, context);});
	    assert.throws(function(){seal_decrypter.decrypt(enc_data);});
	    enc_data[2]++;
	    assert.throws(function(){seal_decrypter.decrypt(enc_data, context);});
	    enc_data[2]--;
	    dec_data = seal_decrypter.decrypt(enc_data, context);
	    assert.throws(function(){seal_encrypter.encrypt(empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){seal_decrypter.decrypt(empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	});
	it("seal (without context)", function(){
	    seal_encrypter = new addon.SecureCellSeal(password);
	    seal_decrypter = new addon.SecureCellSeal(password);
	    seal_intruder_decrypter = new addon.SecureCellSeal(new Buffer("This is test password1"));
	    enc_data = seal_encrypter.encrypt(message);
	    dec_data = seal_decrypter.decrypt(enc_data);
	    assert.equal(message.toString(), dec_data.toString());
	    assert.throws(function(){seal_intruder_decrypter.decrypt(enc_data);});
	    enc_data[2]++;
	    assert.throws(function(){seal_decrypter.decrypt(enc_data);});
	    enc_data[2]--;
	    dec_data = seal_decrypter.decrypt(enc_data);
	    assert.throws(function(){seal_encrypter.encrypt(empty_message)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){seal_decrypter.decrypt(empty_message)}, expect_code(addon.INVALID_PARAMETER));
	});
	it("context imprint", function(){
	    context_imprint_encrypter = new addon.SecureCellContextImprint(password);
	    context_imprint_decrypter = new addon.SecureCellContextImprint(password);
	    context_imprint_intruder_decrypter = new addon.SecureCellContextImprint(new Buffer("This is test password1"));
	    assert.throws(function(){new addon.SecureCellContextImprint(empty_message)});
	    context_imprint_enc_data = context_imprint_encrypter.encrypt(message, context);
	    assert.equal(message.length, context_imprint_enc_data.length);	    
	    context_imprint_dec_data = context_imprint_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.equal(message.toString(), context_imprint_dec_data.toString());
	    context_imprint_dec_data = context_imprint_intruder_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.notEqual(message.toString(), context_imprint_dec_data.toString());
	    assert.throws(function(){context_imprint_encrypter.encrypt(empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){context_imprint_encrypter.encrypt(message, empty_message)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){context_imprint_decrypter.decrypt(empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){context_imprint_decrypter.decrypt(context_imprint_enc_data, empty_message)}, expect_code(addon.INVALID_PARAMETER));
	    context_imprint_enc_data[2]++;
	    context_imprint_dec_data = context_imprint_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.notEqual(message.toString(), context_imprint_dec_data.toString());
	});
	it("token protect", function(){
	    encrypter = new addon.SecureCellTokenProtect(password);
	    decrypter = new addon.SecureCellTokenProtect(password);
	    intruder_decrypter = new addon.SecureCellTokenProtect(new Buffer("This is test password1"));
	    assert.throws(function(){new addon.SecureCellTokenProtect(empty_message)});
	    enc_data = encrypter.encrypt(message, context);
	    assert.equal(message.length, enc_data.data.length);
	    dec_data = decrypter.decrypt(enc_data.data, enc_data.token, context);
	    assert.equal(message.toString(), dec_data.toString());
	    assert.throws(function(){intruder_decrypter.decrypt(enc_data.data, enc_data.token)});
	    assert.throws(function(){encrypter.encrypt(empty_message)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){encrypter.encrypt(empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){decrypter.decrypt(empty_message, enc_data.token, context)}, expect_code(addon.INVALID_PARAMETER));
	    assert.throws(function(){decrypter.decrypt(enc_data.data, empty_message, context)}, expect_code(addon.INVALID_PARAMETER));
	    enc_data.data[2]++;
	    assert.throws(function(){decrypter.decrypt(enc_data.data, enc_data.token, context)});
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
	it("invalid parameters", function(){
	    empty_secret = new Buffer("");
	    assert.throws(function(){new addon.SecureComparator(empty_secret)}, expect_code(addon.INVALID_PARAMETER));
	    server_comparator = new addon.SecureComparator(server_secret);
	    assert.throws(function(){server_comparator.proceedCompare(empty_message)}, expect_code(addon.INVALID_PARAMETER));
	})
    })
})

describe("jsthemis", function(){
    describe("key generation", function(){
        it("generates and joins", function(){
            pair = new addon.KeyPair()
            new_pair = new addon.KeyPair(pair.private(), pair.public())
            assert.deepStrictEqual(pair.private(), new_pair.private())
            assert.deepStrictEqual(pair.public(), new_pair.public())
            assert.throws(function(){new addon.KeyPair(empty_message, empty_message)}, expect_code(addon.INVALID_PARAMETER));
        })
    })
})
