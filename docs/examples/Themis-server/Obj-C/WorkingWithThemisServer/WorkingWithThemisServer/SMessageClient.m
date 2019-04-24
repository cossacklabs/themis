#import <objcthemis/smessage.h>
#import <objcthemis/skeygen.h>

#import "SMessageClient.h"

// ---------------------- IMPORTANT SETUP ---------------------------------------

// User id and Server Public Key should be copied from the Server Setup Page
// https://docs.cossacklabs.com/simulator/interactive/

static NSString * kSimulatorURL = @"https://docs.cossacklabs.com/api/";

// looks like "UMHHCkqYkSNyeee"
static NSString * kUserId = @"<user id>";

// Server public key ("server key"),
// looks like "VUeeeeMgAAAC0AgP/5Aj+YKbf9Vt6vzW9gAudxMy8IZ+wdNj/L9aJN7lJk6WiI"
static NSString * kServerPublicKey = @"<server public key>";

// These are default keys, you can re-generate them by running `- (void)generateClientKeys`
// Copy and paste `kClientPublicKey` to Server Setup Page.
// RE-GENERATE these keys before using your app in production.
static NSString * kClientPrivateKey = @"UkVDMgAAAC201GYXADuazso18cvilLzojlZgben5OohakQYUr3hJ+EiQw/xF";
static NSString * kClientPublicKey = @"VUVDMgAAAC0IpZHJAp6MORixTEmgX8VrUghTyoGb/h7WiEqhelcYXepMnsHa";

// ---------------------- END OF SETUP ---------------------------------------

@interface SMessageClient ()
@end


@implementation SMessageClient


- (void)postRequestTo:(NSString *)stringURL message:(NSData *)message completion:(void (^)(NSData * data, NSError * error))completion {

    NSURL * url = [NSURL URLWithString:stringURL];
    NSURLSessionConfiguration * config = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config];

    NSMutableURLRequest * request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"POST";
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-type"];

    NSString * base64URLEncodedMessage = [[message base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed] stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet alphanumericCharacterSet]];
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


- (void)runSecureMessageCITest {

// ---------------------- KEY GENERATION ---------------------------------------

    // uncomment to re-generate keys
//    [self generateClientKeys];
//    return;

// ---------------------- END KEY GENERATION -----------------------------------

    [self checkKeysNotEmpty];

    NSData * serverPublicKey = [[NSData alloc] initWithBase64EncodedString:kServerPublicKey options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:kClientPrivateKey options:NSDataBase64DecodingIgnoreUnknownCharacters];
    TSMessage * encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:clientPrivateKey
                                                                 peerPublicKey:serverPublicKey];

    NSString * message = @"Hello Themis! Testing your server here..";

    NSError * themisError = nil;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    if (themisError) {
        NSLog(@"encryption error: %@", themisError);
        return;
    }

    NSString * stringURL = [NSString stringWithFormat:@"%@%@/", kSimulatorURL, kUserId];
    [self postRequestTo:stringURL message:encryptedMessage
             completion:^(NSData * data, NSError * error) {
                 if (error || !data) {
                     NSLog(@"response error %@", error);
                     return;
                 }

                 NSError * wrappingError = nil;
                 NSData * unwrap = [encrypter unwrapData:data error:&wrappingError];
                 if (wrappingError) {
                     NSLog(@"decryption error: %@", wrappingError);
                     return;
                 }

                 NSLog(@"Unwraped response %@", [[NSString alloc] initWithData:unwrap encoding:NSUTF8StringEncoding]);

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
    NSAssert(![kUserId isEqualToString:@"<user id>"], @"Get user id from https://docs.cossacklabs.com/simulator/interactive//");

    NSAssert(![kServerPublicKey isEqualToString:@"<server public key>"], @"Get server key from https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kClientPrivateKey isEqualToString:@"<generated client private key>"], @"Generate client keys by running `[self generateClientKeys]` or obtain from server https://docs.cossacklabs.com/simulator/interactive/");

    NSAssert(![kClientPublicKey isEqualToString:@"<generated client public key>"], @"Generate client keys by running `[self generateClientKeys]` or obtain from server https://docs.cossacklabs.com/simulator/interactive/");
}

@end
