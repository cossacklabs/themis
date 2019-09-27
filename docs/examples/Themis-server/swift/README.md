0. Install Pods
1. Run app, see that it's crashing on asserts. This is fine :)
2. Open `AppDelegate` and select example to start: SecureMessage or SecureSession. Just uncomment one of the modes. 


![appdelegate](pics/appdelegate.png)


## SecureSession mode

Let's assume you want to play with SecureSession.

3. Open `SSessionClient` file.

### Client key generation

4. Find these lines:

```swift
// ---------------------- KEY GENERATION ---------------------------------------

        // uncomment to re-generate keys
//         generateClientKeys()
//         return;
        
// ---------------------- END KEY GENERATION -----------------------------------
```
5. Uncomment key generation and run example. You should see output in console, like this:

```swift
 ------------ running SSession Client example 
EC privateKey = UkVDMgAAAC0lV2hOAKFJZyAOLFkzavGzxP8v0Byrs9r5N4u1rm4qYSTLqf+2
EC publicKey = VUVDMgAAAC1hdEZpAlxrxhaWHW23X9ILnXgeZRNHRJ2lt/w4e4tdzablr/WS
```

6. Copy keys to text file :)
7. Comment key generation lines (`generateClientKeys()`) function, we don't need them anymore.
8. Find client keys constants in code and paste generated keys, like this:

```swift 
let kClientPrivateKey: String = "UkVDMgAAAC0lV2hOAKFJZyAOLFkzavGzxP8v0Byrs9r5N4u1rm4qYSTLqf+2"
let kClientPublicKey: String = "VUVDMgAAAC1hdEZpAlxrxhaWHW23X9ILnXgeZRNHRJ2lt/w4e4tdzablr/WS"
```
    
### Server key generation

The idea is to let server know client public key, and let client know server id and public key.

 
9. Open https://docs.cossacklabs.com/simulator/interactive/

- Copy User ID from server interface to `kUserId` in code.
- Copy Server ID from server interface to `kServerId` in code.
- Copy Server Key from server interface to `kServerPublicKey` in code.
- Paste `kClientPublicKey` to Client public key in server interface.


![sever dashboard](pics/server-dashboard.png)

Code will look like this:

```swift
let kUserId: String = "gUOosYEgpqbGXJf"
let kServerId: String = "shfVaKYXvlbAMoD"
let kServerPublicKey: String = "VUVDMgAAAC2TKF6QAyMjQXtKmAAfNOSeW6D1BeXKx6XIX8bWFG3Tfjo74ldm"
    
let kClientPrivateKey: String = "UkVDMgAAAC0lV2hOAKFJZyAOLFkzavGzxP8v0Byrs9r5N4u1rm4qYSTLqf+2"
let kClientPublicKey: String = "VUVDMgAAAC1hdEZpAlxrxhaWHW23X9ILnXgeZRNHRJ2lt/w4e4tdzablr/WS"
```

### Start all the things!

10. Start server in SecureSession mode (press corresponded button).
11. Run app on SecureSession mode.
   - don't forget to uncomment SecureSession mode in AppDelegate
   - don't forget to comment out key generation
   
12. Enjoy


## Secure Message mode

Do the same steps as described above, but for `SMessageClient` file :) Run server in Secure Message mode.


## Read more!

Read How Themis Server Simulator works:
https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
