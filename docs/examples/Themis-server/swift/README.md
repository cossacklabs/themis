0. Install Pods
1. Run app, see that it's crashing on asserts
2. Select example to start: SecureMessage or SecureSession
3. Generate client keys. The easiest way: uncomment first lines in `runSecureMessageCITest/runSecureSessionCITest`. Copy keys from console and fill-in kClientPrivateKey and kClientPublicKey vars. 
4. Obtain server keys. Open https://themis.cossacklabs.com/interactive-simulator/setup/ and copy UserId (aka ClientId), ServerId, ServerPublic key. Paste ClientPublicKey in the form.
5. Start server in SMessage OR SSession mode (press corresponded button).
6. Start app on SMessage OR Session mode (comment undeeded line in AppDelegate).
7. Enjoy


Read How Themis Server Simulator works:
https://github.com/cossacklabs/themis/wiki/Using-Themis-Server