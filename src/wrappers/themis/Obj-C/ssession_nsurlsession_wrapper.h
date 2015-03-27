/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "objcthemis/ssession.h"
#import "objcthemis/error.h"

@interface Transport: SSession_transport_interface
{
  NSData* _client_pub_key;
}

-(instancetype)init;
-(NSData*)get_public_key: (NSData*)Id error:(NSError**)errorPtr;

@end

@interface SecureSession_NSURLSession_wrapper: NSObject
{
  SSession* _ssession;
  NSURLSession* _session;
  NSURL* _url;
}

-(instancetype)initWithId: (NSString*)id andPrivKey:(NSData*)priv_key andServerUrl:(NSURL*)url;

-(void) connect: (NSError**)errorPtr;
-(void) send_clean: (NSData*)message error:(NSError**)errorPtr;
-(void) send: (NSData*)message error:(NSError**)errorPtr;
-(void) on_receive_data: (NSData*)message error:(NSError*)error;
-(void) on_receive:(NSData*) message error:(NSError*)error;
@end
