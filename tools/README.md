# Integration tests for Themis

Here we keep the tools for automated testing of Themis
across the supported platforms.
The tools are accessible to humans, too :)

To run these tools:
1. Make sure you've [installed the wrapper](https://docs.cossacklabs.com/pages/documentation-themis/#installing-themis-wrappers) for the language you want to use (if that language is not Rust or Go - in that case, there is nothing you need to additionally install).  
2. Run the test tool with the specific command for
your language.

| Wrapper    | How to run a test tool                |
| ---------- | ------------------------------------- |
| GoThemis   | `go run tools/go/keygen.go`           |
| JsThemis   | `node tools/js/jsthemis/keygen.js`    |
| PHPThemis  | `php -f tools/php/keygen.php`         |
| PyThemis   | `python tools/python/keygen.py`       |
| RbThemis   | `ruby tools/ruby/keygen.rb`           |
| RustThemis | `cargo run --example keygen_tool --`  |
| WasmThemis | `node tools/js/wasm-themis/keygen.js` |

## Available tools

- **keygen_tool** —
  a tool for generating ECDSA keys
- <b>scell_*</b> —
  encrypt or decrypt a string using Secure Cell
  - **scell_context_string_echo** —
    context mode
  - **scell_token_string_echo** —
    token protect mode
  - **scell_context_string_echo** —
    context imprint mode
  - **scell_seal_string_echo_pw** — 
    sealing mode with passphrase
- **smessage_encryption** —
  encrypt, decrypt, sign, and verify a string using Secure Message


### keygen_tool

A tool for generating key files usable for other tools.  
Remember that the sender key is always a private key while the recipient key is always public.

We recommend using ECDSA keys. Themis library supports RSA keys, too, as well, but `keygen_tool` does not generate them (and there is no built-in "switch" to alternate between ECDSA keys and RSA keys).

Usage:  
    `run tool [<private-key-file> <public-key-file>]`  
If paths to key files are not specified then keys will be placed into
"key" and "key.pub" in the current directory.

### scell_*_string_echo

A family of command-line tools used for testing Secure Cell.

All of them accept plaintext input and produce base64-encoded encrypted output
(or vice versa for decryption).
The _associated context_ can be provided as an optional last argument.

Token protect mode produces and accepts _two_ strings separated by a comma:
the encrypted data followed by the authentication token. 

Usage:  
    `run tool {enc|dec} <key> <message> [<context>]`  
Commands:    
- `enc` - encrypt
- `dec` - decrypt

Example:

```
$ run --example scell_token_string_echo -- enc password input
KEYSbKY=,AAEBQAwAAAAQAAAABQAAAEPGcrB2ftqZT7fDEZYMS1ab3+iLGoOOAx/D3X4=

$ run --example scell_token_string_echo -- dec password KEYSbKY=,AAEBQAwAAAAQAAAABQAAAEPGcrB2ftqZT7fDEZYMS1ab3+iLGoOOAx/D3X4=
input
```

### smessage_encryption

A universal tool for testing Secure Message.

The first argument determines the action to perform. Commands:  

- `enc`: encrypt message
- `dec`: decrypt message
- `sign`: sign message
- `verify`: verify signature

It needs to be followed by the paths to files with a pair of private and public keys.
Suitable keys can be generated using the **keygen_tool**.

Finally, specify either:
- a message to encrypt or sign as plaintext
- a base64-encoded ciphertext to decrypt or verify
