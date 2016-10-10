var addon = require('jsthemis');
var assert = require('assert');


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
	    assert.throws(function(){intruder_decrypter.decrypt(encrypted_message);});
	});
	it("sign/verify", function(){
	    signed_message=encrypter.sign(message);
	    assert.equal(message.toString(), decrypter.verify(signed_message).toString());
	    assert.equal(message.toString(), intruder_decrypter.verify(signed_message).toString());
	    signed_message[2]++;
	    assert.throws(function(){decrypter.verify(signed_message);});
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
    })
})

describe("jsthemis", function(){
    describe("secure cell", function(){
	message=new Buffer("This is test message");
	password=new Buffer("This is test password");
	context=new Buffer("This is test context");
	it("seal (with context)", function(){
	    seal_encrypter = new addon.SecureCellSeal(password);
	    seal_decrypter = new addon.SecureCellSeal(password);
	    seal_intruder_decrypter = new addon.SecureCellSeal(new Buffer("This is test password1"));
	    enc_data = seal_encrypter.encrypt(message, context);
	    dec_data = seal_decrypter.decrypt(enc_data, context);
	    assert.equal(message.toString(), dec_data.toString());
	    assert.throws(function(){seal_intruder_decrypter.decrypt(enc_data, context);});
	    assert.throws(function(){seal_decrypter.decrypt(enc_data);});
	    enc_data[2]++;
	    assert.throws(function(){seal_decrypter.decrypt(enc_data, context);});
	    enc_data[2]--;
	    dec_data = seal_decrypter.decrypt(enc_data, context);
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
	});
	it("context imprint", function(){
	    context_imprint_encrypter = new addon.SecureCellContextImprint(password);
	    context_imprint_decrypter = new addon.SecureCellContextImprint(password);
	    context_imprint_intruder_decrypter = new addon.SecureCellContextImprint(new Buffer("This is test password1"));
	    context_imprint_enc_data = context_imprint_encrypter.encrypt(message, context);
	    assert.equal(message.length, context_imprint_enc_data.length);	    
	    context_imprint_dec_data = context_imprint_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.equal(message.toString(), context_imprint_dec_data.toString());
	    context_imprint_dec_data = context_imprint_intruder_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.notEqual(message.toString(), context_imprint_dec_data.toString());
	    context_imprint_enc_data[2]++;
	    context_imprint_dec_data = context_imprint_decrypter.decrypt(context_imprint_enc_data, context);
	    assert.notEqual(message.toString(), context_imprint_dec_data.toString());
	});
	it("token protect", function(){
	    encrypter = new addon.SecureCellTokenProtect(password);
	    decrypter = new addon.SecureCellTokenProtect(password);
	    intruder_decrypter = new addon.SecureCellTokenProtect(new Buffer("This is test password1"));
	    enc_data = encrypter.encrypt(message, context);
	    assert.equal(message.length, enc_data.data.length);
	    dec_data = decrypter.decrypt(enc_data.data, enc_data.token, context);
	    assert.equal(message.toString(), dec_data.toString());
	    assert.throws(function(){intruder_decrypter.decrypt(enc_data.data, enc_data.token)});
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
    })
})
