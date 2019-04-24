//
//  SSessionClient.swift
//  SwiftThemisServerExample
//
//  Created by Anastasi Voitova on 19.04.16.
//  Copyright © 2016 CossackLabs. All rights reserved.
//

import Foundation
import themis

final class Transport: TSSessionTransportInterface {
    
    fileprivate var serverId: String?
    fileprivate var serverPublicKeyData: Data?
    
    func setupKeys(_ serverId: String, serverPublicKey: String) {
        self.serverId = serverId
        self.serverPublicKeyData = Data(base64Encoded: serverPublicKey,
                                          options: .ignoreUnknownCharacters)
    }
    
    override func publicKey(for binaryId: Data!) throws -> Data {
        let error: Error = NSError(domain: "com.themisserver.example", code: -1, userInfo: nil)
        let stringFromData = String(data: binaryId, encoding: String.Encoding.utf8)
        if stringFromData == nil {
            throw error
        }
        
        if stringFromData == serverId {
            guard let resultData: Data = serverPublicKeyData else {
                throw error
            }
            return resultData
        }
        // it should be nil, but swift ¯\_(ツ)_/¯
        return Data()
    }
}


final class SSessionClient {
    
    var transport: Transport?
    var session: TSSession?
    
    // ---------------------- IMPORTANT SETUP ---------------------------------------
    // Read how Themis Server works:
    // https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
    
    // You may NEED to re-generate all keys
    
    // User id, Server id and Server Public Key should be copied from the Server Setup Page
    // https://docs.cossacklabs.com/simulator/interactive/
    
    let kServerURL = "https://docs.cossacklabs.com/api/"
    
    let kUserId = "<user id>"
    let kServerId = "<server id>"
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
        
        let base64URLEncodedMessage: String = message.base64EncodedString(options: .endLineWithLineFeed)
                .addingPercentEncoding(withAllowedCharacters: CharacterSet.alphanumerics)!
        
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
    
    
    fileprivate func startSessionTo(_ stringURL: String, message: Data, completion: @escaping (_ error: Error?) -> Void) {
        postRequestTo(stringURL, message: message, completion: {(data: Data?, error: Error?) -> Void in
            guard let data = data else {
                print("Error occurred while starting session \(error as Optional)")
                return
            }
            
            do {
                guard let decryptedMessage = try self.session?.unwrapData(data) else {
                    throw NSError(domain: "com.themisserver.example", code: -4, userInfo: nil)
                }
                
                if let session = self.session, session.isSessionEstablished() == true {
                    print("Session established!")
                    completion(nil)
                } else {
                    self.startSessionTo(stringURL, message: decryptedMessage, completion: completion)
                }
            } catch let error {
                // frustrating, but 'unwrapData' can return nil without error (and it's okay)
                // Swift returns error "Foundation._GenericObjCError"
                if let session = self.session, session.isSessionEstablished() == true {
                    print("Session established!")
                    completion(nil)
                } else {
                    print("Error occurred while decrypting session start message \(error)", #function)
                    completion(error)
                }
                return
            }
        })
    }
    
    
    fileprivate func sendMessageTo(_ stringURL: String, message: String, completion: @escaping (_ data: String?, _ error: Error?) -> Void) {
        var encryptedMessage: Data
        do {
            guard let wrappedMessage: Data = try self.session?.wrap(message.data(using: String.Encoding.utf8)) else {
                print("Error occurred during wrapping message ", #function)
                return
            }
            encryptedMessage = wrappedMessage
        } catch let error {
            print("Error occurred while wrapping message \(error)", #function)
            completion(nil, error)
            return
        }
        
        postRequestTo(stringURL, message: encryptedMessage, completion: {(data: Data?, error: Error?) -> Void in
            guard let data = data else {
                print("Error occurred while sending message \(error as Optional)")
                return
            }
            
            do {
                guard let decryptedMessage: Data = try self.session?.unwrapData(data),
                        let resultString: String = String(data: decryptedMessage, encoding: String.Encoding.utf8) else {
                            
                    throw NSError(domain: "com.themisserver.example", code: -3, userInfo: nil)
                }
                completion(resultString, nil)
                
            } catch let error {
                print("Error occurred while decrypting message \(error)", #function)
                completion(nil, error)
                return
            }
        })
    }
    
    
    func runSecureSessionCITest() {
// ---------------------- KEY GENERATION ---------------------------------------
        
        // uncomment to re-generate keys
//         generateClientKeys()
//         return;
        
// ---------------------- END KEY GENERATION -----------------------------------
        
        checkKeysNotEmpty()
        
        guard let clientIdData: Data = kUserId.data(using: String.Encoding.utf8),
            let clientPrivateKey: Data = Data(base64Encoded: kClientPrivateKey,
                                                  options: .ignoreUnknownCharacters) else {
                                                    
            print("Error occurred during base64 encoding", #function)
            return
        }
        
        self.transport = Transport()
        self.transport?.setupKeys(kServerId, serverPublicKey: kServerPublicKey)
        self.session = TSSession(userId: clientIdData, privateKey: clientPrivateKey, callbacks: self.transport!)

        
        var connectionMessage: Data
        do {
            guard let resultOfConnectionRequest = try session?.connectRequest() else {
                throw NSError(domain: "com.themisserver.example", code: -2, userInfo: nil)
            }
            
            connectionMessage = resultOfConnectionRequest
        } catch let error {
            print("Error occurred while connecting to session \(error)", #function)
            return
        }
        
        
        let stringURL: String = "\(kServerURL)\(kUserId)/"
        self.startSessionTo(stringURL, message: connectionMessage, completion: {(error: Error?) -> Void in
            if error != nil {
                print("Error occurred while session initialization \(error as Optional)", #function)
                return
            }
            
            self.sendMessageTo(stringURL, message: "This is test message from Swift",
                completion: {(data: String?, messageError: Error?) -> Void in
                
                guard let data = data else {
                    print("Error occurred while sending message \(messageError as Optional)", #function)
                    return
                }
                print("Response success:\n\(data)")
            })
        })
    }
    
    
    fileprivate func generateClientKeys() {
        // Use Client Public Key to run server (copy and paste Client Public Key to the Setup page)
        // https://themis.cossacklabs.com/interactive-simulator/setup/
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
        
        print("EC privateKey = \(privateKeyECString)")
        print("EC publicKey = \(publicKeyECString)")
    }
    
    
    fileprivate func checkKeysNotEmpty() {
        // Read how Themis Server works:
        // https://docs.cossacklabs.com/pages/documentation-themis/#interactive-simulator-themis-server
        
        
        assert(!(kUserId == "<user id>"), "Get user id from https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kServerId == "<server id>"), "Get server id from https://docs.cossacklabs.com/simulator/interactive/")
        
        assert(!(kServerPublicKey == "<server public key>"), "Get server key from https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kClientPrivateKey == "<generated client private key>"), "Generate client keys by running `generateClientKeys()` or obtain from server https://docs.cossacklabs.com/simulator/interactive/")
        assert(!(kClientPublicKey == "<generated client public key>"), "Generate client keys by running `generateClientKeys()` or obtain from server https://docs.cossacklabs.com/simulator/interactive/")
    }
}
