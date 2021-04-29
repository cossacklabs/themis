/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.cossacklabs.themis;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/**
 * Secure Cell for data storage.
 * <p>
 * <strong>Secure Cell</strong> is a high-level cryptographic service
 * aimed at protecting arbitrary data stored in various types of storage
 * (e.g., databases, filesystem files, document archives, cloud storage, etc.)
 * It provides both strong symmetric encryption and data authentication mechanism.
 * <p>
 * The general approach is that given:
 *
 * <ul>
 *   <li><em>input:</em> some source data to protect</li>
 *   <li><em>secret:</em> symmetric key or a password</li>
 *   <li><em>context:</em> and an optional "context information"</li>
 * </ul>
 *
 * Secure Cell will produce:
 *
 * <ul>
 *   <li><em>cell:</em> the encrypted data</li>
 *   <li><em>authentication token:</em> some authentication data</li>
 * </ul>
 *
 * <p>
 * The purpose of the optional context information (e.g., a database row number or file name)
 * is to establish a secure association between this context and the protected data.
 * In short, even when the secret is known, if the context is incorrect then decryption will fail.
 * <p>
 * The purpose of the authentication data is to validate that given a correct key or passphrase
 * (and context), the decrypted data is indeed the same as the original source data.
 * <p>
 * The authentication data must be stored somewhere. The most convenient way is to simply
 * append it to the encrypted data, but this is not always possible due to the storage
 * architecture of your application. Secure Cell offers variants that address this issue
 * in different ways.
 * <p>
 * By default, Secure Cell uses AES-256 for encryption.
 * Authentication data takes additional 44 bytes when symmetric keys are used
 * and 70 bytes in case the data is secured with a passphrase.
 * <p>
 * Secure Cell supports 2 kinds of secrets:
 *
 * <ul>
 *   <li>
 *     <strong>Symmetric keys</strong> are convenient to store and efficient to use for machines.
 *     However, they are relatively long and hard for humans to remember.
 *     <p>
 *     New symmetric keys can be generated with {@link SymmetricKey}.
 *   <li>
 *     <strong>Passphrases</strong>, in contrast, can be shorter and easier to remember.
 *     <p>
 *     However, passphrases are typically much less random than keys.
 *     Secure Cell uses a <em>key derivation function</em> (KDF) to compensate for that
 *     and achieves security comparable to keys with shorter passphrases.
 *     This comes at a significant performance cost though.
 * </ul>
 *
 * Use {@code ...WithKey} or {@code ...WithPassphrase} methods
 * to construct a Secure Cell with a particular kind of secret in particular mode.
 * <p>
 * Secure Cell supports 3 operation modes:
 *
 * <ul>
 *   <li>
 *     {@link SecureCell.Seal} mode is the most secure and easy to use.
 *     Your best choice most of the time.
 *     This is also the only mode that supports passphrases at the moment.
 *   <li>
 *     {@link SecureCell.TokenProtect} mode is just as secure, but a bit harder to use.
 *     This is your choice if you need to keep authentication data separate.
 *   <li>
 *     {@link SecureCell.ContextImprint} mode is a length-preserving version of Secure Cell
 *     with no additional data stored. Should be used carefully.
 * </ul>
 *
 * The operation mode is selected via an appropriate method of {@code SecureCell}.
 * For example, {@link SecureCell#SealWithKey(SymmetricKey)} constructs Secure Cell in Seal mode
 * using a symmetric key.
 * <p>
 * <a href="https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/">Here you can learn more</a>
 * about the underlying considerations, limitations, and features of each mode.
 *
 * @since JavaThemis 0.9
 */
public class SecureCell {
	
	static {
		System.loadLibrary("themis_jni");
	}

    /**
     * Secure Cell in <em>Seal</em> operation mode.
     * <p>
     * This is the most secure and easy way to protect stored data.
     * The data is protected by a symmetric key or a passphrase.
     * <p>
     * Secure Cell in Seal mode will encrypt the data and append an "authentication tag"
     * with auxiliary security information, forming a single sealed container.
     * This means that the encrypted data will be longer than the original input.
     * <p>
     * Additionally, it is possible to bind the encrypted data to some "associated context"
     * (for example, database row number).
     * In this case decryption of the data with incorrect context will fail
     * (even if the correct key is known and the data has not been tampered).
     * This establishes a cryptographically secure association between the protected data
     * and the context in which it is used.
     * With database row numbers this prevents the attacker from swapping encrypted password hashes
     * in the database so the system will not accept credentials of a different user.
     * <p>
     * Security of symmetric key operation mode depends on the quality of the key,
     * with short and incorrectly generated keys being easier to crack.
     * You can use {@link SymmetricKey} to generate good random keys of sufficient length,
     * then use {@link SecureCell#SealWithKey(SymmetricKey)} to construct a Secure Cell.
     * <p>
     * If you need to use a short and easy to remember passphrase,
     * use passphrase API instead: {@link SecureCell#SealWithPassphrase(String)}.
     * <p>
     * You can read more about Seal mode
     * <a href="https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#seal-mode">in documentation</a>.
     *
     * @since JavaThemis 0.13
     */
    public interface Seal {
        /**
         * Encrypts the provided message with associated context.
         * <p>
         * Data is encrypted and authentication token is appended to form a single sealed buffer.
         * Use {@link #decrypt(byte[], byte[])} to decrypt the result later.
         * <p>
         * The context is cryptographically mixed with the data
         * but not included into the resulting encrypted message.
         * You will have to provide the same context again during decryption.
         * Usually this is some plaintext data associated with encrypted data,
         * such as database row number, protocol message ID, etc.
         *
         * @param data      data to encrypt, must not be empty
         * @param context   associated context, may be empty or {@code null}
         *
         * @return encrypted data as a single buffer
         * @throws InvalidArgumentException if data is empty
         * @throws RuntimeException on internal encryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        byte[] encrypt(byte[] data, @Nullable byte[] context);

        /**
         * Encrypts the provided message.
         * <p>
         * Data is encrypted and authentication token is appended to form a single sealed buffer.
         * Use {@link #decrypt(byte[])} to decrypt the result later.
         * <p>
         * This call is equivalent to {@link #encrypt(byte[], byte[])} with an empty associated context.
         *
         * @param data  data to encrypt, must not be empty
         *
         * @return encrypted data as a single buffer
         * @throws InvalidArgumentException if data is empty
         * @throws RuntimeException on internal encryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        byte[] encrypt(byte[] data);

        /**
         * Decrypts the provided message with associated context.
         * <p>
         * Secure Cell validates association with the context data, decrypts the message,
         * and verifies its integrity using authentication data embedded into the message.
         * <p>
         * You need to provide the same context data as provided to {@link #encrypt(byte[], byte[])}.
         * You can also decrypt data encrypted with {@link #encrypt(byte[])} by using an empty context.
         *
         * @param data      encrypted data, cannot be empty
         * @param context   associated context, may be empty or {@code null}
         *
         * @return decrypted data if everything goes well
         * @throws SecureCellException if decryption failed: for example,
         *         if the data has been tampered with, if the secret is incorrect,
         *         or if the associated context does not match the one used to encrypt the data
         * @throws InvalidArgumentException if data is empty
         */
        @NotNull
        @Contract(pure = true)
        byte[] decrypt(byte[] data, @Nullable byte[] context) throws SecureCellException;

        /**
         * Decrypts the provided message with associated context.
         * <p>
         * Secure Cell decrypts the message and verifies its integrity
         * using authentication data embedded into the message.
         * <p>
         * Use this method to decrypt data encrypted with {@link #encrypt(byte[])}.
         * If you use associated context, call {@link #decrypt(byte[], byte[])} instead.
         *
         * @param data  encrypted data, cannot be empty
         *
         * @return decrypted data if everything goes well
         * @throws SecureCellException if decryption failed: for example,
         *         if the data has been tampered with, if the secret is incorrect,
         *         or if the associated context does not match the one used to encrypt the data
         * @throws InvalidArgumentException if data is empty
         */
        @NotNull
        @Contract(pure = true)
        byte[] decrypt(byte[] data) throws SecureCellException;
    }

    /**
     * Makes a new Secure Cell in Seal mode secured by symmetric key.
     *
     * @param key symmetric key to use
     *
     * @return a new {@link SecureCell.Seal} instance
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static Seal SealWithKey(SymmetricKey key) {
        if (key == null) {
            throw new NullArgumentException("key cannot be null");
        }
        return new SecureCellSeal(key);
    }

    /**
     * Makes a new Secure Cell in Seal mode secured by symmetric key.
     *
     * @param key symmetric key bytes to use
     *
     * @return a new {@link SecureCell.Seal} instance
     * @throws InvalidArgumentException if {@code key} is empty
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static Seal SealWithKey(byte[] key) {
        return new SecureCellSeal(new SymmetricKey(key));
    }

    /**
     * Makes a new Secure Cell in Seal mode secured by passphrase.
     * <p>
     * The passphrase will be encoded in UTF-8 for compatibility with other platforms.
     * Use {@link #SealWithPassphrase(String, Charset)} if you need other encoding.
     *
     * @param passphrase passphrase to use
     *
     * @return a new {@link SecureCell.Seal} instance
     * @throws NullArgumentException if passphrase is null
     * @throws InvalidArgumentException if passphrase is empty
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static Seal SealWithPassphrase(String passphrase) {
        return new SecureCellSealWithPassphrase(passphrase, StandardCharsets.UTF_8);
    }

    /**
     * Makes a new Secure Cell in Seal mode secured by passphrase.
     *
     * @param passphrase passphrase to use
     * @param charset    how to encode the passphrase
     *
     * @return a new {@link SecureCell.Seal} instance
     * @throws NullArgumentException if passphrase is null
     * @throws InvalidArgumentException if passphrase is empty
     * @throws RuntimeException if passphrase cannot be encoded in given encoding
     *         without data loss ({@link CharacterCodingException is the cause})
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_, _ -> new", pure = true)
    public static Seal SealWithPassphrase(String passphrase, Charset charset) {
        return new SecureCellSealWithPassphrase(passphrase, charset);
    }

    /**
     * Makes a new Secure Cell in Seal mode secured by passphrase.
     * <p>
     * Use this method if you need a custom encoding of the passphrase,
     * already have it encoded, or use a binary passphrase.
     *
     * @param encodedPassphrase passphrase bytes to use
     *
     * @return a new {@link SecureCell.Seal} instance
     * @throws NullArgumentException if passphrase is null
     * @throws InvalidArgumentException if passphrase is empty
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static Seal SealWithPassphrase(byte[] encodedPassphrase) {
        return new SecureCellSealWithPassphrase(encodedPassphrase);
    }

    /**
     * Secure Cell in <em>Token Protect</em> operation mode.
     * <p>
     * This is a modified Seal mode for constrained environments.
     * The data is protected by a symmetric key.
     * <p>
     * Token Protect mode is designed for cases when underlying storage constraints
     * do not allow the size of the data to grow (so {@link SecureCell.Seal} cannot be used).
     * However, if you have access to a different storage location
     * (e.g., another table in the database) where additional security parameters can be stored
     * then Token Protect mode can be used instead of Seal mode.
     * <p>
     * Token Protect mode produces authentication tag and other auxiliary data
     * (aka "authentication token") in a detached buffer.
     * This keeps the original size of the encrypted data
     * while enabling separate storage of security information.
     * Note that the same token must be provided along with the correct secret
     * and matching associated context in order for the data to be decrypted successfully.
     * <p>
     * Since {@link SecureCell.TokenProtect} uses the same security parameters as {@link SecureCell.Seal}
     * (just stored in a different location), these modes have the same highest security level.
     * Token Protect mode only requires slightly more programming effort
     * in exchange for preserving the original data size.
     * <p>
     * Security of symmetric key operation mode depends on the quality of the key,
     * with short and incorrectly generated keys being easier to crack.
     * You can use {@link SymmetricKey} to generate good random keys of sufficient length,
     * then use {@link SecureCell#TokenProtectWithKey(SymmetricKey)} to construct a Secure Cell.
     * <p>
     * You can read more about Token Protect mode
     * <a href="https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#token-protect-mode">in documentation</a>.
     *
     * @since JavaThemis 0.13
     */
    public interface TokenProtect {
        /**
         * Encrypts the provided message with associated context.
         * <p>
         * Data is encrypted and authentication token is produced separately.
         * Use {@link #decrypt(byte[], byte[], byte[])} to decrypt the result later,
         * providing both encrypted data and corresponding token.
         * <p>
         * The context is cryptographically mixed with the data
         * but not included into the resulting encrypted message.
         * You will have to provide the same context again during decryption.
         * Usually this is some plaintext data associated with encrypted data,
         * such as database row number, protocol message ID, etc.
         *
         * @param data      data to encrypt, must not be empty
         * @param context   associated context, may be empty or {@code null}
         *
         * @return an object containing encrypted data and authentication token
         * @throws InvalidArgumentException if data is empty
         * @throws RuntimeException on internal encryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        SecureCellData encrypt(byte[] data, @Nullable byte[] context);

        /**
         * Encrypts the provided message with associated context.
         * <p>
         * Data is encrypted and authentication token is produced separately.
         * Use {@link #decrypt(byte[], byte[])} to decrypt the result later,
         * providing both encrypted data and corresponding token.
         * <p>
         * This call is equivalent to {@link #encrypt(byte[], byte[])} with an empty associated context.
         *
         * @param data  data to encrypt, must not be empty
         *
         * @return an object containing encrypted data and authentication token
         * @throws InvalidArgumentException if data is empty
         * @throws RuntimeException on internal encryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        SecureCellData encrypt(byte[] data);

        /**
         * Decrypts the provided message with associated context.
         * <p>
         * Secure Cell validates association with the context data, decrypts the message,
         * and verifies its integrity using the provided authentication token.
         * <p>
         * You need to provide the same context data as provided to {@link #encrypt(byte[], byte[])}.
         * You can also decrypt data encrypted with {@link #encrypt(byte[])} by using an empty context.
         *
         * @param data      encrypted data, cannot be empty
         * @param token     authentication token, cannot be empty
         * @param context   associated context, may be empty or {@code null}
         *
         * @return decrypted data if everything goes well
         * @throws SecureCellException if decryption failed: for example,
         *         if the data has been tampered with, if the secret is incorrect,
         *         or if the associated context does not match the one used to encrypt the data
         * @throws InvalidArgumentException if data or token is empty
         */
        @NotNull
        @Contract(pure = true)
        byte[] decrypt(byte[] data, byte[] token, @Nullable  byte[] context) throws SecureCellException;

        /**
         * Decrypts the provided message with associated context.
         * <p>
         * Secure Cell decrypts the message and verifies its integrity
         * using the provided authentication token.
         * <p>
         * Use this method to decrypt data encrypted with {@link #encrypt(byte[])}.
         * If you use associated context, call {@link #decrypt(byte[], byte[], byte[])} instead.
         *
         * @param data  encrypted data, cannot be empty
         * @param token authentication token, cannot be empty
         *
         * @return decrypted data if everything goes well
         * @throws SecureCellException if decryption failed: for example,
         *         if the data has been tampered with, if the secret is incorrect,
         *         or if the associated context does not match the one used to encrypt the data
         * @throws InvalidArgumentException if data or token is empty
         */
        @NotNull
        @Contract(pure = true)
        byte[] decrypt(byte[] data, byte[] token) throws SecureCellException;
    }

    /**
     * Makes a new Secure Cell in Token Protect mode secured by symmetric key.
     *
     * @param key symmetric key to use
     *
     * @return a new {@link SecureCell.TokenProtect} instance
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static TokenProtect TokenProtectWithKey(SymmetricKey key) {
        if (key == null) {
            throw new NullArgumentException("key cannot be null");
        }
        return new SecureCellTokenProtect(key);
    }

    /**
     * Makes a new Secure Cell in Token Protect mode secured by symmetric key.
     *
     * @param key symmetric key bytes to use
     *
     * @return a new {@link SecureCell.TokenProtect} instance
     * @throws InvalidArgumentException if {@code key} is empty
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static TokenProtect TokenProtectWithKey(byte[] key) {
        return new SecureCellTokenProtect(new SymmetricKey(key));
    }

    /**
     * Secure Cell in <em>Context Imprint</em> operation mode.
     * <p>
     * This is an advanced mode for constrained environments.
     * The data is protected by a symmetric key.
     * <p>
     * Context Imprint mode is intended for environments where storage constraints
     * do not allow the size of the data to grow and there is no auxiliary storage available.
     * Context Imprint mode requires an additional "associated context"
     * to be provided along with the key in order to protect the data.
     * <p>
     * In Context Imprint mode no authentication token is computed or verified.
     * This means that integrity of the data is not enforced,
     * so the overall security level is slightly lower than in Seal or Token Protect modes.
     * <p>
     * Security of symmetric key operation mode depends on the quality of the key,
     * with short and incorrectly generated keys being easier to crack.
     * You can use {@link SymmetricKey} to generate good random keys of sufficient length.
     * then use {@link SecureCell#ContextImprintWithKey(SymmetricKey)} to construct a Secure Cell.
     * <p>
     * To ensure the highest security level possible,
     * supply a different associated context for each encryption invocation with the same key.
     * <p>
     * You can read more about Context Imprint mode
     * <a href="https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#context-imprint-mode">in documentation</a>.
     *
     * @since JavaThemis 0.13
     */
    public interface ContextImprint {
        /**
         * Encrypts the provided message with associated context.
         * <p>
         * Data is encrypted and combined with the provided context.
         * Use {@link #decrypt(byte[], byte[])} to decrypt the result later.
         * <p>
         * The context is cryptographically mixed with the data
         * but not included into the resulting encrypted message.
         * You will have to provide the same context again during decryption.
         * Usually this is some plaintext data associated with encrypted data,
         * such as database row number, protocol message ID, etc.
         *
         * @param data      data to encrypt, must not be empty
         * @param context   associated context, must not be empty
         *
         * @return encrypted data of the same size as input
         * @throws InvalidArgumentException if data or context is empty
         * @throws RuntimeException on internal encryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        byte[] encrypt(byte[] data, byte[] context);

        /**
         * Decrypts the provided message with associated context.
         * <p>
         * Secure Cell validates association with the context data and decrypts the message.
         * You need to provide the same context data as provided to {@link #encrypt(byte[], byte[])}.
         *
         * @param data      encrypted data, cannot be empty
         * @param context   associated context, cannot be empty
         *
         * @return decrypted data if everything goes well
         * <p>
         * Note that in Context Imprint mode messages do not include any authentication token
         * for integrity validation.
         * If data has been corrupted or the context is incorrect,
         * Secure Cell will most likely successfully return corrupted non-empty output.
         *
         * @throws InvalidArgumentException if data or context is empty
         * @throws RuntimeException on internal decryption failure
         *         (with {@link SecureCellException} as its cause)
         */
        @NotNull
        @Contract(pure = true)
        byte[] decrypt(byte[] data, byte[] context);
    }

    /**
     * Makes a new Secure Cell in Context Imprint mode secured by symmetric key.
     *
     * @param key symmetric key to use
     *
     * @return a new {@link SecureCell.ContextImprint} instance
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static ContextImprint ContextImprintWithKey(SymmetricKey key) {
        if (key == null) {
            throw new NullArgumentException("key cannot be null");
        }
        return new SecureCellContextImprint(key);
    }

    /**
     * Makes a new Secure Cell in Context Imprint mode secured by symmetric key.
     *
     * @param key symmetric key bytes to use
     *
     * @return a new {@link SecureCell.ContextImprint} instance
     * @throws InvalidArgumentException if {@code key} is empty
     *
     * @since JavaThemis 0.13
     */
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static ContextImprint ContextImprintWithKey(byte[] key) {
        return new SecureCellContextImprint(new SymmetricKey(key));
    }

	/**
	 * Creates new SecureCell in specified mode
	 * @param mode SecureCell mode
	 * @throws InvalidArgumentException when unsupported mode is specified
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Select the mode with an appropriate static factory method instead:
	 * <ul>
	 *   <li>{@link #SealWithKey(SymmetricKey)}
	 *   <li>{@link #TokenProtectWithKey(SymmetricKey)}
	 *   <li>{@link #ContextImprintWithKey(SymmetricKey)}
	 * </ul>
	 */
	@Deprecated
	public SecureCell(int mode) {

		if (mode < MODE_SEAL || mode > MODE_CONTEXT_IMPRINT) {
			throw new InvalidArgumentException("invalid mode");
		}
		
		this.mode = mode;
	}
	
	/**
	 * Creates new SecureCell with default master key in SEAL mode
	 * @param key master key
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use {@link #SealWithKey(byte[])} instead.
	 */
	@Deprecated
	public SecureCell(byte[] key) {
		this.key = key;
	}
	
	/**
	 * Creates new SecureCell with default master key in specified mode
	 * @param key master key
	 * @param mode SecureCell mode
	 * @throws InvalidArgumentException when unsupported mode is specified
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use an appropriate static factory method instead:
	 * <ul>
	 *   <li>{@link #SealWithKey(byte[])}
	 *   <li>{@link #TokenProtectWithKey(byte[])}
	 *   <li>{@link #ContextImprintWithKey(byte[])}
	 * </ul>
	 */
	@Deprecated
	public SecureCell(byte[] key, int mode) {
		this(mode);
		this.key = key;
	}
	
	/**
	 * Creates new SecureCell with default master password in SEAL mode
	 * @param password master password
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method is <strong>not secure</strong> when used with short passphrases or passwords.
	 * Do not use it in new code.
	 * <p>
	 * If you need to use short passphrases, consider {@link #SealWithPassphrase(String)}.
	 * Otherwise consider using symmetric keys: {@link #SealWithKey(SymmetricKey)}.
	 * <p>
	 * This method does not have direct counterpart in the new API.
	 * It is equivalent to {@code SecureCell.SealWithKey(password.getBytes("UTF-16"))}.
	 */
	@Deprecated
	public SecureCell(String password) {
		this(password.getBytes(CHARSET));
	}
	
	/**
	 * Creates new SecureCell with default master password in specified mode
	 * @param password master password
	 * @param mode Secure Cell mode
	 * @throws InvalidArgumentException when unsupported mode is specified
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method is <strong>not secure</strong> when used with short passphrases or passwords.
	 * Do not use it in new code.
	 * <p>
	 * If you need to use short passphrases, consider {@link #SealWithPassphrase(String)}.
	 * Otherwise consider using symmetric keys:
	 * <ul>
	 *   <li>{@link #SealWithKey(byte[])}
	 *   <li>{@link #TokenProtectWithKey(byte[])}
	 *   <li>{@link #ContextImprintWithKey(byte[])}
	 * </ul>
	 * This method does not have direct counterpart in the new API.
	 * It is equivalent to {@code SecureCell.SealWithKey(password.getBytes("UTF-16"))}
	 * or corresponding other mode.
	 */
	@Deprecated
	public SecureCell(String password, int mode) {
		this(mode);
		this.key = password.getBytes(CHARSET);
	}
	
	int mode = MODE_SEAL;
	byte[] key;
	
	static final Charset CHARSET = StandardCharsets.UTF_16;

	/** @deprecated since JavaThemis 0.13 */
	@Deprecated
	public static final int MODE_SEAL = 0;
	/** @deprecated since JavaThemis 0.13 */
	@Deprecated
	public static final int MODE_TOKEN_PROTECT = 1;
	/** @deprecated since JavaThemis 0.13 */
	@Deprecated
	public static final int MODE_CONTEXT_IMPRINT = 2;
	static final int MODE_SEAL_PASSPHRASE = 3;
	
	static native byte[][] encrypt(byte[] key, byte[] context, byte[] data, int mode);
	static native byte[] decrypt(byte[] key, byte[] context, byte[][] protectedData, int mode);

	static SecureCellData protect(byte[] key, byte[] context, byte[] data, int mode) throws SecureCellException {

		if (null == key) {
			throw new NullArgumentException("Master key was not provided");
		}
		
		if (null == data) {
			throw new NullArgumentException("Data was not provided");
		}
		
		if (MODE_CONTEXT_IMPRINT == mode) {
			// Context is mandatory for this mode
			if (null == context) {
				throw new NullArgumentException("Context is mandatory for context imprint mode");
			}
		}
		
		byte[][] protectedData = encrypt(key, context, data, mode);
		
		if (null == protectedData) {
			throw new SecureCellException();
		}
		
		return new SecureCellData(protectedData[0], protectedData[1]);
	}

	static byte[] unprotect(byte[] key, byte[] context, SecureCellData protectedData, int mode) throws SecureCellException {

		if (null == key) {
			throw new NullArgumentException("Master key was not provided");
		}
		
		if (null == protectedData) {
			throw new NullArgumentException("Protected data was not provided");
		}
		
		if (MODE_CONTEXT_IMPRINT == mode) {
			// Context is mandatory for this mode
			if (null == context) {
				throw new NullArgumentException("Context is mandatory for context imprint mode");
			}
		}
		
		if (null == protectedData.getProtectedData()) {
			throw new InvalidArgumentException("protectedData");
		}
		
		if (MODE_TOKEN_PROTECT == mode) {
			if (null == protectedData.getAdditionalData()) {
				throw new InvalidArgumentException("additionalData");
			}
		}
		
		byte[] data = decrypt(key, context, new byte[][]{protectedData.getProtectedData(), protectedData.getAdditionalData()}, mode);
		if (null == data) {
			throw new SecureCellException();
		}
		
		return data;
	}
	
	/**
	 * Protects data with specified master key
	 * @param key master key to use for protecting data
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code encrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public SecureCellData protect(byte[] key, byte[] context, byte[] data) throws SecureCellException {
		return protect(key, context, data, this.mode);
	}
	
	/**
	 * Protects data with default master key
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when default master key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code encrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public SecureCellData protect(byte[] context, byte[] data) throws SecureCellException {
		return this.protect(this.key, context, data);
	}
	
	/**
	 * Protects data with specified master password
	 * @param password master password to use for protecting data
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method is <strong>not secure</strong> when used with short passphrases or passwords.
	 * Do not use it in new code.
	 * <p>
	 * Consider using new construction API with passphrases instead.
	 */
	@Deprecated
	public SecureCellData protect(String password, String context, byte[] data) throws SecureCellException {
		return this.protect(password.getBytes(CHARSET), context.getBytes(CHARSET), data);
	}
	
	/**
	 * Protects data with default master password
	 * @param context to which protected data will be bound (may be null)
	 * @param data to protect
	 * @return SecureCellData with protected data
	 * @throws NullArgumentException when key or data is null
	 * @throws SecureCellException when cannot protect the data
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code encrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public SecureCellData protect(String context, byte[] data) throws SecureCellException {
		return this.protect(this.key, context.getBytes(CHARSET), data);
	}
	
	/**
	 * Decrypts and verifies protected data
	 * @param key master key
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code decrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public byte[] unprotect(byte[] key, byte[] context, SecureCellData protectedData) throws SecureCellException {
		return unprotect(key, context, protectedData, this.mode);
	}
	
	/**
	 * Decrypts and verifies protected data with default master key
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code decrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public byte[] unprotect(byte[] context, SecureCellData protectedData) throws SecureCellException {
		return this.unprotect(this.key, context, protectedData);
	}
	
	/**
	 * Decrypts and verifies protected data
	 * @param password master password
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * This method is <strong>not secure</strong> when used with short passphrases or passwords.
	 * Do not use it in new code.
	 * <p>
	 * Consider using new construction API with passphrases instead.
	 */
	@Deprecated
	public byte[] unprotect(String password, String context, SecureCellData protectedData) throws SecureCellException {
		return this.unprotect(password.getBytes(CHARSET), context.getBytes(CHARSET), protectedData);
	}
	
	/**
	 * Decrypts and verifies protected data with default master password
	 * @param context to which protected data will is bound (may be null, must be same as provided in protect call)
	 * @param protectedData to verify
	 * @return original data
	 * @throws NullArgumentException when key or protectedData is null
	 * @throws SecureCellException when cannot decrypt protectedData
	 * @throws InvalidArgumentException when protectedData is incorrect
	 * @deprecated since JavaThemis 0.13
	 * <p>
	 * Use new construction API and {@code decrypt(byte[], byte[])} instead.
	 */
	@Deprecated
	public byte[] unprotect(String context, SecureCellData protectedData) throws SecureCellException {
		return this.unprotect(this.key, context.getBytes(CHARSET), protectedData);
	}
}
