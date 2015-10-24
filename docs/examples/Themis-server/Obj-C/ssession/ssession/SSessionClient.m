//
//  SSessionClient.m
//  ssession
//
//  Created by Admin on 24.10.15.
//  Copyright © 2015 Cossacklabs. All rights reserved.
//

#import <objcthemis/ssession.h>

#import "SSessionClient.h"

NSString * const client_id = @"ctvQJJmkJgWHshS";
NSString * const server_id = @"yIougRzafACYbLX";
NSString * const client_priv_key=@"UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x";
NSString * const server_pub_key=@"VUVDMgAAAC2ieKIHAriuJ82jtTOemCHk9fAcIrdqdpVPTwNQK0QmZjegGYKU";

@interface Transport : TSSessionTransportInterface

@end

@implementation Transport

- (NSData *)publicKeyFor:(NSData *)binaryId error:(NSError **)error {
    NSString * stringFromData = [[NSString alloc] initWithData:binaryId encoding:NSUTF8StringEncoding];
    if ([stringFromData isEqualToString:server_id]) {
        NSData * key = [[NSData alloc] initWithBase64EncodedString:server_pub_key options:NSDataBase64DecodingIgnoreUnknownCharacters];
        return key;
    }
    return nil;
}

@end

@interface SSessionClient()

@property (nonatomic, strong) Transport * transport;
@property (nonatomic, strong) TSSession * session;

@end

@implementation SSessionClient

- (void)postRequestTo:(NSString *)stringURL message:(NSData *)message completion:(void (^)(NSData * data, NSError * error))completion {
    
    NSURL *url = [NSURL URLWithString:stringURL];
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config];
    
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"POST";
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-type"];
    
    NSString* bodystr=[[[NSString stringWithFormat:@"%@%@", @"message=", [message base64EncodedStringWithOptions:0]] stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]] stringByReplacingOccurrencesOfString:@"+" withString:@"%2B"];
    NSLog(@"Request: %@", bodystr );
    NSData* body = [bodystr dataUsingEncoding:NSUTF8StringEncoding];
    
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

- (void) initSession:(NSString*) url connectMessage:(NSData*) connectMessage completion:(void (^)(NSError * error))completion{
    [self postRequestTo:url message:connectMessage
             completion:^(NSData * data, NSError * error) {
                 if (error || !data) {
                     NSLog(@"response error %@", error);
                     return;
                 }
                 NSError * wrappingError = nil;
                 NSData * unwrap = [ self.session unwrapData:data error:&wrappingError];
                 if (wrappingError) {
                     completion(wrappingError);
                     return;
                 }
                 if([self.session isSessionEstablished]){
                     completion(nil);
                 }
                 else {
                     [self initSession:url connectMessage:unwrap completion:completion];
                 }
                 
             }];
}

- (void) sendMessage:(NSString*) url message:(NSString*) message completion:(void (^)(NSString * data, NSError * error))completion{
    NSError * error=nil;
    NSData* encdata = [self.session wrapData:[message dataUsingEncoding:NSUTF8StringEncoding] error:&error];
    if(error){
        NSLog(@"wrapping error %@", error);
        return;
    }
    [self postRequestTo:url message:encdata completion:^(NSData * data, NSError * error) {
                 if (error || !data) {
                     NSLog(@"response error %@", error);
                     return;
                 }
                 NSError * wrappingError = nil;
                 NSData * unwrap = [ self.session unwrapData:data error:&wrappingError];
                 if (wrappingError) {
                     completion(nil, wrappingError);
                     return;
                 }
                 completion([[NSString alloc] initWithData:unwrap encoding:NSUTF8StringEncoding], nil);
             }];
}
- (void) secureSessionCITest{
    self.transport = [Transport new];
    self.session = [[TSSession alloc] initWithUserId:[client_id dataUsingEncoding:NSUTF8StringEncoding] privateKey:[[NSData alloc] initWithBase64EncodedString:client_priv_key options:0] callbacks: self.transport ];
    NSError * error=nil;
    NSData * conn_message=[self.session connectRequest:&error];
    if(error){
        NSLog(@"Error: %@", error);
        return;
    }
    
    NSString * stringURL = [NSString stringWithFormat:@"%@%@/", @"https://themis.cossacklabs.com/api/", client_id];
    [self initSession:stringURL
       connectMessage:conn_message
           completion:^(NSError * error) {
                 if (error) {
                     NSLog(@"response error %@", error);
                     return;
                 }
               [self sendMessage:stringURL message:@"This is test message" completion:^(NSString* data, NSError * error){
                   if(error){
                       NSLog(@"response error 2 %@", error);
                       return;
                   }
                   NSLog(@"Response: %@", data);
               }];
             }];
}

@end