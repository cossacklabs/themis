var addon = require('./build/Release/jsthemis.node');
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
	    try {
		intruder_decrypter.decrypt(encrypted_message);
		assert.equal(-1,0);
	    } catch (exception_var) {}
	});
	it("sign/verify", function(){
	    signed_message=encrypter.sign(message);
	    assert.equal(message.toString(), decrypter.verify(signed_message).toString());
	    assert.equal(message.toString(), intruder_decrypter.verify(signed_message).toString());
	    signed_message[10]++;
	    try {
		decrypter.verify(signed_message);
		assert.equal(-1,0);		
	    } catch (exception_var) {}
	})
    })
})

describe("jsthemis", function(){
    describe("secure message", function(){
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