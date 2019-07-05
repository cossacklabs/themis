//
//  SMessageClient.swift
//  SwiftThemisServerExample
//
//  Created by Anastasi Voitova on 19.04.16.
//  Copyright © 2016 CossackLabs. All rights reserved.
//

import Foundation
import themis


final class SMessageClient {
    
    // ---------------------- IMPORTANT SETUP ---------------------------------------
    // Read how Themis Server works:
    // https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
    
    // You may NEED to re-generate all keys
    
    // User id and Server Public Key should be copied from the Server Setup Page
    // https://docs.cossacklabs.com/simulator/interactive/
    
    let kServerURL = "https://docs.cossacklabs.com/api/"
    
    let kUserId = "<user id>"
    let kServerPublicKey = "<server public key>"
    
    // These are default keys, you can re-generate them by running `generateClientKeys()`
    // Copy and paste `kClientPublicKey` to Server Setup Page.
    // RE-GENERATE these keys before using your app in production.
    let kClientPrivateKey = "UkVDMgAAAC0TXvcoAGxwWV3QQ9fgds+4pqAWmDqQAfkb0r/+gAi89sggGLpV"
    let kClientPublicKey = "VUVDMgAAAC3mmD1pAuXBcr8k+1rLNYkmw+MwTJMofuDaOTXLf75HW8BIG/5l"
    
    // ---------------------- END OF SETUP ---------------------------------------

    fileprivate func postRequestTo(_ stringURL: String, message: Data, completion: @escaping (_ data: Data?, _ error: Error?) -> Void) {
        let url: URL = URL(string: stringURL)!
        let config: URLSessionConfiguration = URLSessionConfiguration.default
        let session: URLSession = URLSession(configuration: config)
        
        let request: NSMutableURLRequest = NSMutableURLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue("application/x-www-form-urlencoded", forHTTPHeaderField: "Content-type")
        
        let base64URLEncodedMessage: String = message.base64EncodedString(options: .endLineWithLineFeed).addingPercentEncoding(withAllowedCharacters: CharacterSet.alphanumerics)!
        let base64Body: String = "\("message=")\(base64URLEncodedMessage)"
        let body: Data = base64Body.data(using: String.Encoding.utf8)!
        
        let uploadTask: URLSessionDataTask = session.uploadTask(with: request as URLRequest, from: body,
                completionHandler: {(data: Data?, response: URLResponse?, error: Error?) -> Void in
                    
            guard let data = data else {
                print("Oops, response = \(response as Optional)\n error = \(error as Optional)")
                completion(nil, error)
                return
            }
                
            if let response = response as? HTTPURLResponse, response.statusCode != 200 {
                print("Oops, response = \(response)\n error = \(error as Optional)")
                completion(nil, error)
                return
            }
                    
            completion(data, nil)
            return
        })
        
        uploadTask.resume()
    }


    func runSecureMessageCITest() {
        
// ---------------------- KEY GENERATION ---------------------------------------

        // uncomment to re-generate keys
//         generateClientKeys()
//         return;
        
// ---------------------- END KEY GENERATION -----------------------------------
        
        checkKeysNotEmpty()
        
        guard let serverPublicKey: Data = Data(base64Encoded: kServerPublicKey,
                                                   options: .ignoreUnknownCharacters),
            let clientPrivateKey: Data = Data(base64Encoded: kClientPrivateKey,
                                                  options: .ignoreUnknownCharacters) else {
                                                    
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        let encrypter: TSMessage = TSMessage.init(inEncryptModeWithPrivateKey: clientPrivateKey,
                                                  peerPublicKey: serverPublicKey)!
        
        let message: String = "Hello Themis from Swift! Testing your server here ;)"
        
        var encryptedMessage: Data = Data()
        do {
            encryptedMessage = try encrypter.wrap(message.data(using: String.Encoding.utf8))
            let encrypedMessageString = encryptedMessage.base64EncodedString(options: .lineLength64Characters)
            print("encryptedMessage = \(encrypedMessageString)")
            
        } catch let error {
            print("Error occurred while encrypting \(error)", #function)
            return
        }
        
        let stringURL: String = "\(kServerURL)\(kUserId)/"
        postRequestTo(stringURL, message: encryptedMessage, completion: {(data: Data?, error: Error?) -> Void in
            guard let data = data else {
                print("response error \(error as Optional)")
                return
            }        
            
            do {
                let decryptedMessage: Data = try encrypter.unwrapData(data)
                let resultString: String = String(data: decryptedMessage, encoding: String.Encoding.utf8)!
                print("decryptedMessage->\n\(resultString)")
                
            } catch let error {
                print("Error occurred while decrypting \(error)", #function)
                return
            }
        })
    }

    fileprivate func generateClientKeys() {
        // Use Client Public Key to run server (copy and paste Client Public Key to the Setup page)
        // https://docs.cossacklabs.com/simulator/interactive/
        //
        // Use client private key to encrypt your message
        
        guard let keyGeneratorEC: TSKeyGen = TSKeyGen(algorithm: .EC) else {
            print("Error occurred while initializing object keyGeneratorEC", #function)
            return
        }
        let privateKeyEC: Data = keyGeneratorEC.privateKey as Data
        let publicKeyEC: Data = keyGeneratorEC.publicKey as Data
        
        let privateKeyECString = privateKeyEC.base64EncodedString(options: .lineLength64Characters)
        let publicKeyECString = publicKeyEC.base64EncodedString(options: .lineLength64Characters)
        
        print("EC client privateKey = \(privateKeyECString)")
        print("EC client publicKey = \(publicKeyECString)")

    }


    fileprivate func checkKeysNotEmpty() {
        // Read how Themis Server works:
        // https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
        
        
        assert(!(kUserId == "<user id>"), "Get user id from https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kServerPublicKey == "<server public key>"), "Get server key from https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kClientPrivateKey == "<generated client private key>"), "Generate client keys by running `generateClientKeys()` or obtain from server https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kClientPublicKey == "<generated client public key>"), "Generate client keys by running `generateClientKeys()` or obtain from server https://docs.cossacklabs.com/simulator/interactive/")
    }
}
