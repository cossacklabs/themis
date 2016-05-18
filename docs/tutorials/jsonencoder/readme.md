# JSON Encoder

This tutorial shows how to use Themis Secure Cell to protect JSON data with secret-key cryptography. 

Usage scenario: 
- iterate over JSON tree
- for each _k,v_ encrypt each v with: 
  - _password_ as secret key
  - _k_ as context
  
Context is extra security measure, which adds context verification to encryption: you need to supply auxilliary data (frequently it's some identifier for protected data), which you will supply during decryption to verify it's you. JSON key is a very good example of such identifier. 

# Current versions

[Python](https://github.com/cossacklabs/themis/new/master/docs/tutorials/jsonencoder/python/): Python tutorial
