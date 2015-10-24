#import <objcthemis/smessage.h>
#import <objcthemis/ssession.h>
#import <objcthemis/skeygen.h>

#import "SMessageClient.h"


@interface SMessageClient ()

@property (nonatomic, strong) NSData * clientPrivateKey;
@property (nonatomic, strong) NSData * clientPublicKey;

@end

@implementation SMessageClient


- (void)postRequestTo:(NSString *)stringURL message:(NSData *)message completion:(void (^)(NSData * data, NSError * error))completion {
    
    NSURL *url = [NSURL URLWithString:stringURL];
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config];
    
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"POST";
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-type"];
    
    NSString* bodystr=[NSString stringWithFormat:@"%@%@", @"message=", [message base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed]];
    NSData* body = [[bodystr stringByReplacingOccurrencesOfString:@"+" withString:@"2B"] dataUsingEncoding:NSUTF8StringEncoding];
    
    NSURLSessionDataTask * uploadTask = [session uploadTaskWithRequest:request
                                                              fromData:body
                                                     completionHandler:^(NSData * data, NSURLResponse * response, NSError * error) {
                                                         if (error || ((NSHTTPURLResponse *)response).statusCode != 200) {                                                                 NSLog(@"Oops, response %@\nerror: %@", response, error);
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


- (void)secureMessageCITest {
    
    NSString * userid = @"gounDbllopcCcVB";
    NSData * serverPublicKey = [[NSData alloc] initWithBase64EncodedString:@"VUVDMgAAAC2yfQpBAzarlWrwxjlDduEH6Wdfah46bDVLGGCAqYYkVS+dlrcV" options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSData * clientPrivateKey = [[NSData alloc] initWithBase64EncodedString:@"UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x" options:NSDataBase64DecodingIgnoreUnknownCharacters];
    TSMessage * encrypter = [[TSMessage alloc] initInEncryptModeWithPrivateKey:clientPrivateKey
                                                                 peerPublicKey:serverPublicKey];
    
    NSString * message = @"This is test message";
    
    NSError * themisError = nil;
    NSData * encryptedMessage = [encrypter wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&themisError];
    if (themisError) {
        NSLog(@"encryption error: %@", themisError);
        return;
    }
    
    NSString * stringURL = [NSString stringWithFormat:@"%@%@/", @"https://themis.cossacklabs.com/api/", userid];
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
                 
                 NSLog(@"Unwraped response %@",[[NSString alloc] initWithData:unwrap encoding:NSUTF8StringEncoding] );
                 
             }];
}


- (void)generateClientKeys {
    TSKeyGen * keygenEC = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
    
    if (!keygenEC) {
        NSLog(@"%s Error occured while initializing object keygenEC", sel_getName(_cmd));
        return;
    }
    
    self.clientPrivateKey = keygenEC.privateKey;
    self.clientPublicKey = keygenEC.publicKey;
    
    NSLog(@"client private key %@", [self.clientPrivateKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
    NSLog(@"client public key %@", [self.clientPublicKey base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength]);
}
@end
