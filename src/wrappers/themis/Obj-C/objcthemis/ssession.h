/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/21/2015
 * (c) CossackLabs
 */

#import "ssession_transport_interface.h"
#import <themis/themis.h>

@interface SSession : NSObject
{
  secure_session_t* _session;
}

-(instancetype)initWithId: (NSData*)id andPrivateKey:(NSData*)private_key andCallbacks:(SSession_transport_interface*)callbacks;

-(void)connect: (NSError**)errorPtr;
-(NSData*)connect_request: (NSError**)errorPtr;

-(NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;
-(NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;

-(void)send: (NSData*)message error:(NSError**)errorPtr;
-(NSData*)recv: (NSInteger)length error:(NSError**)errorPtr;

@end
