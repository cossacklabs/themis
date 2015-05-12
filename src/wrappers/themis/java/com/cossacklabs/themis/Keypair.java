package com.cossacklabs.themis;

/**
 * Represents Themis asymmetric keypair
 */
public class Keypair {
	
	PrivateKey privateKey;
	PublicKey publicKey;
	
	/**
	 * Creates a new keypair
	 * @param privateKey of the keypair
	 * @param publicKey of the keypair
	 */
	public Keypair(PrivateKey privateKey, PublicKey publicKey) {
		this.privateKey = privateKey;
		this.publicKey = publicKey;
	}
	
	/**
	 * Returns private key of this keypair
	 * @return PrivateKey of the keypair
	 */
	public PrivateKey getPrivateKey() {
		return this.privateKey;
	}
	
	/**
	 * Returns public key of this keypair
	 * @return PublicKey of the keypair
	 */
	public PublicKey getPublicKey() {
		return this.publicKey;
	}

}
