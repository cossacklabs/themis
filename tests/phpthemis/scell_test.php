<?php
    class ScellTest extends PHPUnit_Framework_TestCase{
	/**
         * @dataProvider SealWithContextProvider
         */
	public function testSealWithContext($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);
	    $decrypted_message = phpthemis_scell_seal_decrypt($key, $encrypted_message, $context);
	    $this->assertEquals($decrypted_message, $message);
	}
	

	public function SealWithContextProvider(){
	    return array(
		array(NULL, NULL, NULL, false),
		array("", "", "", false),
		array(NULL, "This is test message", NULL, false),
		array("", "This is test message", "This is test context", false),
		array("This is test key", NULL, NULL, false),
		array("This is test key", "", "This is test context", false),
		array("This is test key", "This is test message", "", true),
		array("This is test key", "This is test message", NULL, true),
		array("This is test key", "This is test message", "This is test context", true)
	    );
	}

	/**
         * @dataProvider SealWithContextProviderNoDecrypt
         */
	public function testSealWithContextNoDecrypt($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message = phpthemis_scell_seal_encrypt($key, $message, $context);
	    $decrypted_message = phpthemis_scell_seal_decrypt($key+"a", $encrypted_message, $context);
	    $this->assertEquals($decrypted_message, $message);
	}

	public function SealWithContextProviderNoDecrypt(){
	    return array(
		array(NULL, NULL, NULL, false),
		array("", "", "", false),
		array(NULL, "This is test message", NULL, false),
		array("", "This is test message", "This is test context", false),
		array("This is test key", NULL, NULL, false),
		array("This is test key", "", "This is test context", false),
		array("This is test key", "This is test message", "", false),
		array("This is test key", "This is test message", NULL, false),
		array("This is test key", "This is test message", "This is test context", false)
	    );
	}

	/**
         * @dataProvider SealWithoutContextProvider
         */
	public function testSealWithoutContext($key, $message, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message = phpthemis_scell_seal_encrypt($key, $message);
	    $decrypted_message = phpthemis_scell_seal_decrypt($key, $encrypted_message);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function SealWithoutContextProvider(){
	    return array(
		array(NULL, NULL, false),
		array("", "", false),
		array(NULL, "This is test message", false),
		array("", "This is test message",  false),
		array("This is test key", NULL, false),
		array("This is test key", "", false),
		array("This is test key", "This is test message", true)
	    );
	}

	/**
         * @dataProvider TokenProtectWithContextProvider
         */
	public function testTokenProtectWithContext($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message =  phpthemis_scell_token_protect_encrypt($key, $message, $context);
	    $decrypted_message =  phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'],  $encrypted_message['token'], $context);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function TokenProtectWithContextProvider(){
	    return array(
		array(NULL, NULL, NULL, false),
		array("", "", "", false),
		array(NULL, "This is test message", NULL, false),
		array("", "This is test message", "This is test context", false),
		array("This is test key", NULL, NULL, false),
		array("This is test key", "", "This is test context", false),
		array("This is test key", "This is test message", "", true),
		array("This is test key", "This is test message", NULL, true),
		array("This is test key", "This is test message", "This is test context", true)
	    );
	}

	/**
         * @dataProvider TokenProtectWithContextProviderNoDecrypt
         */
	public function testTokenProtectWithContextNoDecrypt($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message =  phpthemis_scell_token_protect_encrypt($key, $message, $context);
	    $decrypted_message =  phpthemis_scell_token_protect_decrypt($key+"a", $encrypted_message['encrypted_message'],  $encrypted_message['token'], $context);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function TokenProtectWithContextProviderNoDecrypt(){
	    return array(
		array("This is test key", "This is test message", "", false),
		array("This is test key", "This is test message", NULL, false),
		array("This is test key", "This is test message", "This is test context", false)
	    );
	}

	/**
         * @dataProvider TokenProtectWithContextProviderNoToken
         */
	public function testTokenProtectWithContextNoToken($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message =  phpthemis_scell_token_protect_encrypt($key, $message, $context);
	    $decrypted_message =  phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'],  $encrypted_message['token']+"a", $context);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function TokenProtectWithContextProviderNoToken(){
	    return array(
		array("This is test key", "This is test message", "", false),
		array("This is test key", "This is test message", NULL, false),
		array("This is test key", "This is test message", "This is test context", false)
	    );
	}
	/**
         * @dataProvider TokenProtectWithoutContextProvider
         */
	public function testTokenProtectWithoutContext($key, $message, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message =  phpthemis_scell_token_protect_encrypt($key, $message);
	    $decrypted_message =  phpthemis_scell_token_protect_decrypt($key, $encrypted_message['encrypted_message'],  $encrypted_message['token']);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function TokenProtectWithoutContextProvider(){
	    return array(
		array(NULL, NULL, false),
		array("", "", false),
		array(NULL, "This is test message", false),
		array("", "This is test message",  false),
		array("This is test key", NULL, false),
		array("This is test key", "", false),
		array("This is test key", "This is test message", true)
	    );
	}

	/**
         * @dataProvider ContextImprintProvider
         */
	public function testContextImprint($key, $message, $context, $iscorrect){
	    if(!$iscorrect){
		$this->setExpectedException('Exception');
	    }
	    $encrypted_message =  phpthemis_scell_context_imprint_encrypt($key, $message, $context);
	    $decrypted_message =   phpthemis_scell_context_imprint_decrypt($key, $encrypted_message, $context);
	    $this->assertEquals($decrypted_message, $message);
	}
	
	public function ContextImprintProvider(){
	    return array(
		array(NULL, NULL, NULL, false),
		array("", "", "", false),
		array(NULL, "This is test message", NULL, false),
		array("", "This is test message", "This is test context", false),
		array("This is test key", NULL, NULL, false),
		array("This is test key", "", "This is test context", false),
		array("This is test key", "This is test message", "", false),
		array("This is test key", "This is test message", NULL, false),
		array("This is test key", "This is test message", "This is test context", true)
	    );
	}

    }

?>