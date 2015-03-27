/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "ssession_nsurlsession_wrapper.h"

@implementation Transport

-(instancetype)init{
  self=[super init];
  if(self){
    _client_pub_key=[NSData dataWithContentsOfFile:@"server2.pub"];
  }
  return self;
}

-(NSData*)get_public_key:(NSData*)Id error:(NSError**)errorPtr{
  if([Id isEqualToData:[@"server" dataUsingEncoding:NSUTF8StringEncoding]]){
    return _client_pub_key;
  }
  *errorPtr=SCERROR(TErrorTypeFail, @"client Id not found in db");
  return NULL;
}

@end

@implementation SecureSession_NSURLSession_wrapper

-(instancetype)initWithId: (NSString*)id andPrivKey:(NSData*)priv_key andServerUrl:(NSURL*)url{
  self=[super init];
  if(self){
    Transport* transport=[[Transport alloc] init];
    NSData* id_data=[id dataUsingEncoding:NSUTF8StringEncoding];
    _ssession=[[SSession alloc]initWithId:id_data andPrivateKey:priv_key andCallbacks:transport];
    _session = [NSURLSession sharedSession];
    _url=url;
  }
  return self;
}

-(void) connect: (NSError**) errorPtr{
  NSError* error=NULL;
  NSData* data=[_ssession connect_request:&error];
  if(error){
    NSLog(@"Error occured: %@", error);
    return;
  }
  [self send_clean: data error:&error];
  if(error){
    NSLog(@"Error occured: %@", error);
    return;
  }  
}

-(void) send_clean: (NSData*)message error:(NSError**)errorPtr{
      NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:_url];
      request.HTTPMethod = @"POST";
      NSError* error=NULL;
      request.HTTPBody = message;
      NSURLSessionDataTask * dataTask = [_session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
	  [self on_receive_data:data error:error];
	}];
      [dataTask resume];  
}

-(void) send: (NSData*)message error:(NSError**)errorPtr{
  NSData* wrapped_message=[_ssession wrap: message error:errorPtr];
  if(*errorPtr){
    return;
  }
  [self send_clean:wrapped_message error:errorPtr];
}

-(void) on_receive_data: (NSData*)message error:(NSError*)error_{
  if(error_){
    [self on_receive: message error:error_];
  }
  NSError* error=NULL;
  NSData* unwrapped_message=[_ssession unwrap:message error:&error];
  if(error){
    if([error code]!=TErrorTypeSendAsIs){
      [self on_receive: message error:error];
      return;
    }
    error=NULL;
    [self send_clean: unwrapped_message error:&error];
    if(error){
      [self on_receive: message error:error];
    }
    return;
  }
  [self on_receive: unwrapped_message error:error_];
}

-(void) on_receive: (NSData*)message error:(NSError*)error{
}
@end
