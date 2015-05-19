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

#import "AppDelegate.h"
#import "skeygen.h"
#import "scell_token.h"
#import "scell_seal.h"
#import "scell_context_imprint.h"
#import "smessage.h"


@interface AppDelegate ()

@end


@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // Please, look in debug console to see results

    // Generating/reading keys:
    
    [self runExampleGeneratingKeys];
    [self readingKeysFromFile];

    
    // Secure Message:
    
    [self runExampleSecureMessageEncryptionDecryption];
    
    
    // Secure Cell:
    
    [self runExampleSecureCellSealMode];
    [self runExampleSecureCellTokenProtectMode];
    [self runExampleSecureCellImprint];

    return YES;
}


#pragma mark - Examples


- (NSData *)generateMasterKey {
    NSString * masterKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";
    NSData * masterKeyData = [[NSData alloc] initWithBase64EncodedString:masterKeyString
                                                                 options:NSDataBase64DecodingIgnoreUnknownCharacters];
    return masterKeyData;
}


- (void)runExampleSecureCellSealMode {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    NSData * masterKeyData = [self generateMasterKey];
    SCell_seal * sCellSeal = [[SCell_seal alloc] initWithKey:masterKeyData];

    if (!sCellSeal) {
        NSLog(@"%s Error occured while initializing object sCellSeal", sel_getName(_cmd));
        return;
    }

    NSString * message = @"All your base are belong to us!";
    NSString * context = @"For great justice";
    NSError * themisError;


    // context is optional parameter and may be ignored
    NSData * encryptedMessage = [sCellSeal wrap:[message dataUsingEncoding:NSUTF8StringEncoding]
                                        context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                          error:&themisError];

    if (themisError) {
        NSLog(@"%s Error occured while enrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"encryptedMessage = %@", encryptedMessage);

    NSData * decryptedMessage = [sCellSeal unwrap:encryptedMessage
                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                            error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    NSLog(@"%s resultString = %@", sel_getName(_cmd), resultString);
}


- (void)runExampleSecureCellTokenProtectMode {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));

    NSData * masterKeyData = [self generateMasterKey];
    SCell_token * sCellToken = [[SCell_token alloc] initWithKey:masterKeyData];

    if (!sCellToken) {
        NSLog(@"%s Error occured while initializing object sCellToken", sel_getName(_cmd));
        return;
    }

    NSString * message = @"Roses are grey. Violets are grey.";
    NSString * context = @"I'm a dog";
    NSError * themisError;

    // context is optional parameter and may be ignored
    SCellTokenEncryptedData * encryptedMessage = [sCellToken wrap:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                          context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                            error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while enrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"%s\ncipher = %@:\ntoken = %@", sel_getName(_cmd), [encryptedMessage getCipherText],[encryptedMessage getToken]);

    NSData * decryptedMessage = [sCellToken unwrap:encryptedMessage
                                           context:[context dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    NSLog(@"%s resultString = %@", sel_getName(_cmd), resultString);
}


- (void)runExampleSecureCellImprint {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    NSData * masterKeyData = [self generateMasterKey];
    SCell_context_imprint * contextImprint = [[SCell_context_imprint alloc] initWithKey:masterKeyData];

    if (!contextImprint) {
        NSLog(@"%s Error occured while initializing object contextImprint", sel_getName(_cmd));
        return;
    }

    NSString * message = @"Roses are red. My name is Dave. This poem have no sense";
    NSString * context = @"Microwave";
    NSError * themisError;

    // context is not optional parameter here
    NSData * encryptedMessage = [contextImprint wrapData:[message dataUsingEncoding:NSUTF8StringEncoding]
                                                 context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                   error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while enrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"%@", encryptedMessage);

    // context is not optional parameter here
    NSData * decryptedMessage = [contextImprint unwrapData:encryptedMessage
                                                   context:[context dataUsingEncoding:NSUTF8StringEncoding]
                                                     error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage
                                                    encoding:NSUTF8StringEncoding];
    NSLog(@"%s resultString = %@", sel_getName(_cmd), resultString);
}


- (void)runExampleGeneratingKeys {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    NSData * privateKey;
    NSData * publicKey;

    // Generating RSA keys
    SKeyGen * keygenRSA = [[SKeyGen alloc] init:(RSA)];

    if (!keygenRSA) {
        NSLog(@"%s Error occured while initializing object keygenRSA", sel_getName(_cmd));
        return;
    }

    privateKey = [keygenRSA getPrivKey];
    NSLog(@"RSA private key: %@", privateKey);

    publicKey = [keygenRSA getPubKey];
    NSLog(@"RSA public key:%@", publicKey);

    // Generating EC keys

    SKeyGen * keygenEC = [[SKeyGen alloc] init:(EC)];

    if (!keygenEC) {
        NSLog(@"%s Error occured while initializing object keygenEC", sel_getName(_cmd));
        return;
    }

    privateKey = [keygenEC getPrivKey];
    NSLog(@"EC private key: %@", privateKey);

    publicKey = [keygenEC getPubKey];
    NSLog(@"EC public key:%@", publicKey);
}


- (void)runExampleSecureMessageEncryptionDecryption {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    // ---------- encryption

    // base64 encoded keys:
    // client private key
    // server public key
    NSString * serverPublicKeyString = @"VUVDMgAAAC2ELbj5Aue5xjiJWW3P2KNrBX+HkaeJAb+Z4MrK0cWZlAfpBUql";
    NSString * clientPrivateKeyString = @"UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg";

    NSData * serverPublicKey = [[NSData alloc] initWithBase64EncodedString:serverPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:clientPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];

    // initialize encrypter
    SMessage * encrypter = [[SMessage alloc] initWithPrivateKey:clientPrivateKey peerPublicKey:serverPublicKey];

    NSString * message = @"- Knock, knock.\n- Who’s there?\n*very long pause...*\n- Java.";

    NSError * themisError;
    NSData * encryptedMessage = [encrypter wrap:[message dataUsingEncoding:NSUTF8StringEncoding]
                                          error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while encrypting %@", sel_getName(_cmd), themisError);
        return;
    }
    NSLog(@"%@", encryptedMessage);


    // -------- decryption

    // base64 encoded keys:
    // server private key
    // client public key
    NSString * serverPrivateKeyString = @"UkVDMgAAAC1FsVa6AMGljYqtNWQ+7r4RjXTabLZxZ/14EXmi6ec2e1vrCmyR";
    NSString * clientPublicKeyString = @"VUVDMgAAAC1SsL32Axjosnf2XXUwm/4WxPlZauQ+v+0eOOjpwMN/EO+Huh5d";

    NSData * serverPrivateKey = [[NSData alloc] initWithBase64EncodedString:serverPrivateKeyString
                                                                    options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPublicKey = [[NSData alloc] initWithBase64EncodedString:clientPublicKeyString
                                                                   options:NSDataBase64DecodingIgnoreUnknownCharacters];

    // initialize decrypter
    SMessage * decrypter = [[SMessage alloc] initWithPrivateKey:serverPrivateKey peerPublicKey:clientPublicKey];

    NSData * decryptedMessage = [decrypter unwrap:encryptedMessage error:&themisError];
    if (themisError) {
        NSLog(@"%s Error occured while decrypting %@", sel_getName(_cmd), themisError);
        return;
    }

    NSString * resultString = [[NSString alloc] initWithData:decryptedMessage encoding:NSUTF8StringEncoding];
    NSLog(@"%@", resultString);
}


// Sometimes you will need to read keys from files
- (void)readingKeysFromFile {
    NSLog(@"----------------- %s -----------------", sel_getName(_cmd));
    
    NSData * serverPrivateKeyFromFile = [[NSFileManager defaultManager] contentsAtPath:[[NSBundle mainBundle] pathForResource:@"server"
                                                                                                                       ofType:@"priv"]];
    NSData * serverPublicKeyFromFile = [[NSFileManager defaultManager] contentsAtPath:[[NSBundle mainBundle] pathForResource:@"server"
                                                                                                                      ofType:@"pub"]];
    NSData * clientPrivateKeyOldFromFile = [[NSFileManager defaultManager] contentsAtPath:[[NSBundle mainBundle] pathForResource:@"client"
                                                                                                                          ofType:@"priv"]];
    NSData * clientPublicKeyOldFromFile = [[NSFileManager defaultManager] contentsAtPath:[[NSBundle mainBundle] pathForResource:@"client"
                                                                                                                         ofType:@"pub"]];
    NSLog(@"%s", sel_getName(_cmd));
    NSLog(@"serverPrivateKeyFromFile %@", serverPrivateKeyFromFile);
    NSLog(@"serverPublicKeyFromFile %@", serverPublicKeyFromFile);
    NSLog(@"clientPrivateKeyOldFromFile %@", clientPrivateKeyOldFromFile);
    NSLog(@"clientPublicKeyOldFromFile %@", clientPublicKeyOldFromFile);
}


@end
