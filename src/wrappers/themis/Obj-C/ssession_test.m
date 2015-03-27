/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */


#import "ssession_nsurlsession_wrapper.h"

@interface SecureSession_client: SecureSession_NSURLSession_wrapper

-(instancetype)initWithId: (NSString*)id andPrivKey:(NSData*)priv_key andServerUrl:(NSURL*)url;
-(void) on_receive: (NSData*)message error:(NSError*)error;

@end

@implementation SecureSession_client

-(instancetype)initWithId: (NSString*)id andPrivKey:(NSData*)priv_key andServerUrl:(NSURL*)url{
  self=[super initWithId:id andPrivKey:priv_key andServerUrl:url];
  return self;
}

-(void) on_receive: (NSData*)message error:(NSError*)error_{
  if(error_){
    NSLog(@"Error occured: %@", error_);
    return;
  }
  NSLog(@"received message: %@", [[NSString alloc] initWithData:message encoding:NSUTF8StringEncoding]);
  NSError* error=NULL;
  [self send:[@"Test message" dataUsingEncoding:NSUTF8StringEncoding] error:&error];
  if(error){
    NSLog(@"Error occured: %@", error);
    return;
  }  
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
      NSData* client_priv=[NSData dataWithContentsOfFile:@"client2.priv"];
      SecureSession_client* session=[[SecureSession_client alloc]initWithId:@"client" andPrivKey:client_priv andServerUrl:[NSURL URLWithString:@"http://192.168.144.1:26260"]];
      NSError* error=NULL;
      [session connect: &error];
      if(error){
	NSLog(@"Error occured: %@", error);
	return -4;
      }
      getc(stdin);
    }
    return 0;
}
