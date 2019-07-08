//
//  SSessionClient.m
//  ssession
//
//  Created by Admin on 24.10.15.
//  Copyright © 2015 Cossacklabs. All rights reserved.
//

#import <objcthemis/objcthemis.h>
#import "SSessionClient.h"

// ---------------------- IMPORTANT SETUP ---------------------------------------

// User id, Server id and Server Public Key should be copied from the Server Setup Page
// https://docs.cossacklabs.com/simulator/interactive/

static NSString * kSimulatorURL = @"https://docs.cossacklabs.com/api/";

// looks like "UMHHCkqYkSNyeee"
static NSString * kUserId = @"<user id>";
static NSString * kServerId = @"<server id>";

// Server public key ("server key"),
// looks like "VUeeeeMgAAAC0AgP/5Aj+YKbf9Vt6vzW9gAudxMy8IZ+wdNj/L9aJN7lJk6WiI"
static NSString * kServerPublicKey = @"<server public key>";

// These are default keys, you can re-generate them by running `- (void)generateClientKeys`
// Copy and paste `kClientPublicKey` to Server Setup Page.
// RE-GENERATE these keys before using your app in production.
static NSString * kClientPrivateKey = @"UkVDMgAAAC0Hhj0NAGHnKBAD1D+wk73wnJvKWr8DkceWqQLMBZc32SgkDVdY";
static NSString * kClientPublicKey = @"VUVDMgAAAC3fHpq/AjcipCtQ5JTFGa0QjXLAxvgVN1wZ80GNmGsJXmDh2F3/";

// ---------------------- END OF SETUP ---------------------------------------

@interface Transport : TSSessionTransportInterface

@end


@implementation Transport

- (NSData *)publicKeyFor:(NSData *)binaryId error:(NSError **)error {
    NSString * stringFromData = [[NSString alloc] initWithData:binaryId encoding:NSUTF8StringEncoding];
    if ([stringFromData isEqualToString:kServerId]) {
        NSData * key = [[NSData alloc] initWithBase64EncodedString:kServerPublicKey options:NSDataBase64DecodingIgnoreUnknownCharacters];
        return key;
    }
    return nil;
}

@end


@interface SSessionClient ()

@property (nonatomic, strong) Transport * transport;
@property (nonatomic, strong) TSSession * session;

@end


@implementation SSessionClient

- (void)postRequestTo:(NSString *)stringURL message:(NSData *)message completion:(void (^)(NSData * data, NSError * error))completion {

    NSURL * url = [NSURL URLWithString:stringURL];
    NSURLSessionConfiguration * config = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config];

    NSMutableURLRequest * request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"POST";
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-type"];

    NSString * base64URLEncodedMessage = [[message base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed]
        stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet alphanumericCharacterSet]];
    NSString * base64Body = [NSString stringWithFormat:@"%@%@", @"message=", base64URLEncodedMessage];

    NSData * body = [base64Body dataUsingEncoding:NSUTF8StringEncoding];

    NSURLSessionDataTask * uploadTask = [session uploadTaskWithRequest:request
                                                              fromData:body
                                                     completionHandler:^(NSData * data, NSURLResponse * response, NSError * error) {
                                                         if (error || ((NSHTTPURLResponse *) response).statusCode != 200) {
                                                             NSLog(@"Oops, response %@\nerror: %@", response, error);
                                                             data = nil;
                                                         } else {
                                                             NSLog(@"response %@\n", response);
                                                         }
                                                         if (completion) {
                                                             completion(data, error);
                                                         }
                                                     }];
    [uploadTask resume];
}


- (void)startSessionTo:(NSString *)url connectMessage:(NSData *)connectMessage completion:(void (^)(NSError * error))completion {

    [self postRequestTo:url message:connectMessage
             completion:^(NSData * data, NSError * error) {
                 if (error || !data) {
                     NSLog(@"response error %@", error);
                     return;
                 }
                 NSError * wrappingError = nil;
                 NSData * unwrap = [self.session unwrapData:data error:&wrappingError];
                 if (wrappingError) {
                     completion(wrappingError);
                     return;
                 }
                 if ([self.session isSessionEstablished]) {
                     completion(nil);
                 }
                 else {
                     [self startSessionTo:url connectMessage:unwrap completion:completion];
                 }

             }];
}


- (void)sendMessageTo:(NSString *)url message:(NSString *)message completion:(void (^)(NSString * data, NSError * error))completion {
    NSError * encodeError = nil;
    NSData * encodedData = [self.session wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&encodeError];
    if (encodeError) {
        NSLog(@"wrapping error %@", encodeError);
        return;
    }
    [self postRequestTo:url message:encodedData completion:^(NSData * data, NSError * error) {
        if (error || !data) {
            NSLog(@"response error %@", error);
            return;
        }
        NSError * wrappingError = nil;
        NSData * unwrap = [self.session unwrapData:data error:&wrappingError];
        if (wrappingError) {
            completion(nil, wrappingError);
            return;
        }
        completion([[NSString alloc] initWithData:unwrap encoding:NSUTF8StringEncoding], nil);
    }];
}


- (void)runSecureSessionCITest {

    // ---------------------- KEY GENERATION ---------------------------------------

//    // uncomment to re-generate keys
//    [self generateClientKeys];
//    return;

    // ---------------------- END KEY GENERATION -----------------------------------


    [self checkKeysNotEmpty];

    NSData * userId = [kUserId dataUsingEncoding:NSUTF8StringEncoding];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:kClientPrivateKey options:NSDataBase64DecodingIgnoreUnknownCharacters];

    self.transport = [Transport new];
    self.session = [[TSSession alloc] initWithUserId:userId privateKey:clientPrivateKey callbacks:self.transport];

    NSError * requestError = nil;
    NSData * connectionMessage = [self.session connectRequest:&requestError];
    if (requestError) {
        NSLog(@"Error while connecting %@", requestError);
        return;
    }

    NSString * stringURL = [NSString stringWithFormat:@"%@%@/", kSimulatorURL, kUserId];
    [self startSessionTo:stringURL
          connectMessage:connectionMessage
              completion:^(NSError * error) {
                  if (error) {
                      NSLog(@"response error while session init %@", error);
                      return;
                  }
                  [self sendMessageTo:stringURL message:@"This is test message"
                           completion:^(NSString * data, NSError * messageError) {
                               if (messageError) {
                                   NSLog(@"response messageError %@", messageError);
                                   return;
                               }
                               NSLog(@"Response: %@", data);
                           }];
              }];
}


- (void)generateClientKeys {
    // Use Client Public Key to run server (copy and paste Client Public Key to the Setup page)
    // https://docs.cossacklabs.com/simulator/interactive/
    //
    // Use client private key to encrypt your message
    TSKeyGen * keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];

    if (!keygenEC) {
        NSLog(@"%s Error occurred while initializing object keygenEC", sel_getName(_cmd));
        return;
    }

    NSData * privateKey = keygenEC.privateKey;
    NSData * publicKey = keygenEC.publicKey;

    NSLog(@"client private key %@", [privateKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
    NSLog(@"client public key %@", [publicKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
}


- (void)checkKeysNotEmpty {
    NSAssert(![kUserId isEqualToString:@"<user id>"], @"Get user id from https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kServerId isEqualToString:@"<server id>"], @"Get server id from https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kServerPublicKey isEqualToString:@"<server public key>"], @"Get server key from https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kClientPrivateKey isEqualToString:@"<generated client private key>"], @"Generate client keys by running `[self generateClientKeys]` or obtain from server https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kClientPublicKey isEqualToString:@"<generated client public key>"], @"Generate client keys by running `[self generateClientKeys]` or obtain from server https://docs.cossacklabs.com/simulator/interactive/");
}

@end
