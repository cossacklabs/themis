# JSON encoder, python version

## Using demo app: 

**encrypt**: python application.py -e "shared key" input.json output.json
**decrypt**: python application.py -d "shared key" input.json output.json
**help**: python application.py -h

## Files

`application.py` - application itself
`utils.py` - necessary classes

## Getting started: Iterating JSON with ReverseJSONValue

As an example of iterating over JSON and changing each value there is a class `ReverseJSONValue`, which reverses every value. We try to process every type from [JSON specification](http://json.org/).

Since encoding and decoding in this case perform same functions, decode functions just calls encode one. 

## Applying Themis: encrypting/decrypting with SCellJSONEncoderDecoder

Now we need to modify the encoder to use Secure Cell in Seal mode, which will encrypt all data with the same secret key.

To achieve that, we will:

- in main coder function, wrap return values into self.wrap / self.unwrap when encoding/decoding instead of reversing them:
- create functions wrap/unwrap, which perform base64 conversion and actual encryption
- add key in constructor

Notes on `wrap`/`unwrap`: since Themis works with bytes and we operate with various types, we have to send them in utf-8 strings, which will be converted into bytes, then base64 to be stored within the JSON.

## Getting stronger encryption: encrypting/decrypting with SCellJSONContextAwareEncoderDecoder

Secure Cell Seal mode has one extra layer of security - you can specify special 'context' for each value, which is extra chunk of information (not always secret one), which you need to supply to decrypt the data. Useful context examples are row numbers in table, or keys to values in JSON tree.

Context enables you to verify consistency, and make sure that stealing data together with a key still doesn't give attacker desired results.

Here we've extended encryption/decryption methods with parameter context (but added default state None to presever signature compatibility).




