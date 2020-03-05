Here we keep tools for automated testing of Themis
across the supported platforms.
The tools are accessible to humans, too :).

- **keygen_tool** —
  a tool for generating ECDSA keys 
- <b>scell_*_string_echo</b> —
  encrypt or decrypt a string using Secure Cell
  - **scell_context_string_echo** —
    sealing mode
  - **scell_token_string_echo** —
    token protect mode
  - **scell_context_string_echo** —
    context imprint mode
- **smessage_encryption** —
  encrypt, decrypt, sign, and verify a string using Secure Message

A short reference for using particular tools:

```
$ run --example keygen_tool -- --help
keygen_tool 0.0.3
Generating ECDSA key pairs.

USAGE:
    keygen_tool [ARGS]

FLAGS:
    -h, --help       Prints help information
    -V, --version    Prints version information

ARGS:
    <private>    Private key file (default: key)
    <public>     Public key file (default: key.pub)
```


## keygen_tool

This tool can be used to generate key files usable by other tools.

Themis supports RSA keys for some use cases,
but most features expect ECDSA keys.


## scell_*_string_echo

This is a family of command-line tools used for testing Secure Cell.

All of them accept plaintext input and produce base64-encoded encrypted output
(or vice versa for decryption).
The _user context_ can be provided as an optional last argument.

Token protect mode produces and accepts _two_ strings separated by a comma:
the encrypted data followed by the authentication token. 
Example:

```
$ run --example scell_token_string_echo -- enc password input
KEYSbKY=,AAEBQAwAAAAQAAAABQAAAEPGcrB2ftqZT7fDEZYMS1ab3+iLGoOOAx/D3X4=

$ run --example scell_token_string_echo -- dec password KEYSbKY=,AAEBQAwAAAAQAAAABQAAAEPGcrB2ftqZT7fDEZYMS1ab3+iLGoOOAx/D3X4=
input
```


## smessage_encryption

This is a universal tool used for testing Secure Message.

The first argument determines the action to perform:

- `enc`: encrypt message
- `dec`: encrypt message
- `sign`: sign message
- `verify`: verify signature

It needs to be followed by the paths to files with a pair of private and public keys.
Suitable keys can be generated using the **keygen_tool**.

Finally, specify either:    
- a message to encrypt or sign as plaintext
- a base64-encoded ciphertext to decrypt or verify
