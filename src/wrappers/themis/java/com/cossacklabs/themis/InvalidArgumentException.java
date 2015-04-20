package com.cossacklabs.themis;

public class InvalidArgumentException extends Exception {
	
	public InvalidArgumentException(String message) {
		super("Invalid argument: " + message);
	}

}
