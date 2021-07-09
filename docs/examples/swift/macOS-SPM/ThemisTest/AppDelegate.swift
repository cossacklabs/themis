// Copyright (c) 2021 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import Cocoa
import themis

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        //
        // We don't do UI. Please look into debug console to see the results.
        //

        runExampleSecureCellSealMode()
        runExampleSecureCellTokenProtectMode()
        runExampleSecureCellImprint()

        runExampleGeneratingKeys()
        runExampleReadingKeysFromFile()

        runExampleSecureMessageEncryptionDecryption()
        runExampleSecureMessageSignVerify()

        runExampleSecureComparator()
    }

    // MARK: - Secure Cell

    func generateMasterKey() -> Data {
        let masterKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"
        let masterKeyData: Data = Data(base64Encoded: masterKeyString, options: .ignoreUnknownCharacters)!
        return masterKeyData
    }

    func runExampleSecureCellSealMode() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let cellSeal: TSCellSeal = TSCellSeal(key: masterKeyData) else {
            print("Error occurred while initializing object cellSeal", #function)
            return
        }
        let message: String = "All your base are belong to us!"
        let context: String = "For great justice"

        var encryptedMessage: Data = Data()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellSeal.encrypt(message.data(using: .utf8)!,
                                                 context: context.data(using: .utf8)!)
            print("decryptedMessagez = \(encryptedMessage)")

        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }

        do {
            let decryptedMessage: Data = try cellSeal.decrypt(encryptedMessage,
                                                                 context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")

        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }

    func runExampleSecureCellTokenProtectMode() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let cellToken: TSCellToken = TSCellToken(key: masterKeyData) else {
            print("Error occurred while initializing object cellToken", #function)
            return
        }
        let message: String = "Roses are grey. Violets are grey."
        let context: String = "I'm a dog"

        var encryptedMessage: TSCellTokenEncryptedResult = TSCellTokenEncryptedResult()
        do {
            // context is optional parameter and may be ignored
            encryptedMessage = try cellToken.encrypt(message.data(using: .utf8)!,
                                                  context: context.data(using: .utf8)!)
            print("encryptedMessage.cipher = \(encryptedMessage.encrypted)")
            print("encryptedMessage.token = \(encryptedMessage.token)")

        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }

        do {
            let decryptedMessage: Data = try cellToken.decrypt(encryptedMessage.encrypted,
                                                               token: encryptedMessage.token,
                                                               context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")

        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }

    func runExampleSecureCellImprint() {
        print("----------------------------------", #function)
        let masterKeyData: Data = self.generateMasterKey()
        guard let contextImprint: TSCellContextImprint = TSCellContextImprint(key: masterKeyData) else {
            print("Error occurred while initializing object contextImprint", #function)
            return
        }
        let message: String = "Roses are red. My name is Dave. This poem have no sense"
        let context: String = "Microwave"

        var encryptedMessage: Data = Data()
        do {
            // context is NOT optional parameter here
            encryptedMessage = try contextImprint.encrypt(message.data(using: .utf8)!,
                                                       context: context.data(using: .utf8)!)
            print("encryptedMessage = \(encryptedMessage)")

        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }

        do {
            // context is NOT optional parameter here
            let decryptedMessage: Data = try contextImprint.decrypt(encryptedMessage,
                                                                       context: context.data(using: .utf8)!)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage = \(resultString)")

        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }

    // MARK: - Key Generation and Loading

    func runExampleGeneratingKeys() {
        print("----------------------------------", #function)

        // Generating RSA keys
        guard let keyGeneratorRSA: TSKeyGen = TSKeyGen(algorithm: .RSA) else {
            print("Error occurred while initializing object keyGeneratorRSA", #function)
            return
        }
        let privateKeyRSA: Data = keyGeneratorRSA.privateKey as Data
        let publicKeyRSA: Data = keyGeneratorRSA.publicKey as Data
        print("RSA privateKey = \(privateKeyRSA)")
        print("RSA publicKey = \(publicKeyRSA)")

        // Generating EC keys
        guard let keyGeneratorEC: TSKeyGen = TSKeyGen(algorithm: .EC) else {
            print("Error occurred while initializing object keyGeneratorEC", #function)
            return
        }
        let privateKeyEC: Data = keyGeneratorEC.privateKey as Data
        let publicKeyEC: Data = keyGeneratorEC.publicKey as Data
        print("EC privateKey = \(privateKeyEC)")
        print("RSA publicKey = \(publicKeyEC)")
    }

    // Sometimes you will need to read keys from files
    func runExampleReadingKeysFromFile() {
        print("----------------------------------", #function)
        let fileManager: FileManager = FileManager.default
        let mainBundle: Bundle = Bundle.main

        // yes, app will crash if no keys. that's idea of our sample

        let serverPrivateKeyFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                          ofType: "priv")!)!
        let serverPublicKeyFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "server",
                                                                                         ofType: "pub")!)!
        let clientPrivateKeyOldFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "client",
                                                                                             ofType: "priv")!)!
        let clientPublicKeyOldFromFile: Data = fileManager.contents(atPath: mainBundle.path(forResource: "client",
                                                                                            ofType: "pub")!)!

        print("serverPrivateKeyFromFile = \(serverPrivateKeyFromFile)")
        print("serverPublicKeyFromFile = \(serverPublicKeyFromFile)")
        print("clientPrivateKeyOldFromFile = \(clientPrivateKeyOldFromFile)")
        print("clientPublicKeyOldFromFile = \(clientPublicKeyOldFromFile)")
    }

    // MARK: - Secure Message

    func runExampleSecureMessageEncryptionDecryption() {
        print("----------------------------------", #function)

        // ---------- encryption ----------------

        // base64 encoded keys:
        // client private key
        // server public key

        let serverPublicKeyString: String = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let clientPrivateKeyString: String = "UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg"

        guard let serverPublicKey: Data = Data(base64Encoded: serverPublicKeyString,
                                               options: .ignoreUnknownCharacters),
            let clientPrivateKey: Data = Data(base64Encoded: clientPrivateKeyString,
                                              options: .ignoreUnknownCharacters) else {
                                                print("Error occurred during base64 encoding", #function)
                                                return
        }

        let encrypter: TSMessage = TSMessage.init(inEncryptModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)!

        let message: String = "- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java."

        var encryptedMessage: Data = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: .utf8))
            print("encryptedMessage = \(encryptedMessage)")

        } catch let error as NSError {
            print("Error occurred while encrypting \(error)", #function)
            return
        }

        // ---------- decryption ----------------
        let serverPrivateKeyString: String = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        let clientPublicKeyString: String = "VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d"

        guard let serverPrivateKey: Data = Data(base64Encoded: serverPrivateKeyString,
                                                options: .ignoreUnknownCharacters),
            let clientPublicKey: Data = Data(base64Encoded: clientPublicKeyString,
                                             options: .ignoreUnknownCharacters) else {
                                                print("Error occurred during base64 encoding", #function)
                                                return
        }

        let decrypter: TSMessage = TSMessage.init(inEncryptModeWithPrivateKey: serverPrivateKey,
                                                  peerPublicKey: clientPublicKey)!

        do {
            let decryptedMessage: Data = try decrypter.unwrapData(encryptedMessage)
            let resultString: String = String(data: decryptedMessage, encoding: .utf8)!
            print("decryptedMessage->\n\(resultString)")

        } catch let error as NSError {
            print("Error occurred while decrypting \(error)", #function)
            return
        }
    }

    func runExampleSecureMessageSignVerify() {
        print("----------------------------------", #function)
        
        // base64 encoded keys:
        // private key
        // public key
        
        let publicKeyString: String = "VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql"
        let privateKeyString: String = "UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR"
        
        guard let publicKey: Data = Data(base64Encoded: publicKeyString,
                                         options: .ignoreUnknownCharacters),
            let privateKey: Data = Data(base64Encoded: privateKeyString,
                                        options: .ignoreUnknownCharacters) else {
                                            print("Error occurred during base64 encoding", #function)
                                            return
        }
        
        // ---------- signing ----------------
        // use private key
        
        let signer: TSMessage = TSMessage.init(inSignVerifyModeWithPrivateKey: privateKey,
                                               peerPublicKey: nil)!
        
        let message: String = "I had a problem so I though to use Java. Now I have a ProblemFactory."
        
        var signedMessage: Data = Data()
        do {
            signedMessage = try signer.wrap(message.data(using: .utf8))
            print("signedMessage = \(signedMessage)")
            
        } catch let error as NSError {
            print("Error occurred while signing \(error)", #function)
            return
        }
        
        // ---------- verification ----------------
        // use public key
        let verifier = TSMessage.init(inSignVerifyModeWithPrivateKey: nil,
                                      peerPublicKey: publicKey)!
        
        do {
            let verifiedMessage: Data = try verifier.unwrapData(signedMessage)
            let resultString: String = String(data: verifiedMessage, encoding: .utf8)!
            print("verifiedMessage ->\n\(resultString)")
            
        } catch let error as NSError {
            print("Error occurred while verifing \(error)", #function)
            return
        }
    }

    // MARK: - Secure Comparator

    func runExampleSecureComparator() {
        print("----------------------------------", #function)

        let sharedMessage = "shared secret"
        let client: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!
        let server: TSComparator = TSComparator.init(messageToCompare: sharedMessage.data(using: .utf8)!)!

        // send this message to server
        var data = try? client.beginCompare()

        while (client.status() == TSComparatorStateType.notReady ||
            server.status() == TSComparatorStateType.notReady ) {

                // receive from server
                data = try? server.proceedCompare(data)

                // proceed and send again
                data = try? client.proceedCompare(data)
        }

        if (client.status() == TSComparatorStateType.match) {
            // secrets match
            print("SecureComparator secrets match")
        } else {
            // secrets don't match
            print("SecureComparator secrets do not match")
        }
    }
}
