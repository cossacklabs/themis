/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/21/2015
 * (c) CossackLabs
 */

#import "ssession_transport_interface.h"

@interface SSession : SSession_transport_interface
{
  NSData* _id;
  NSData* _private_key;
  NSObject* _callbacks;
}

-(instancetype)initWithId: (NSData*)Id andPrivateKey:(NSData*)private_key andCallbacks(NSObject*)callbacks;

-(void)connect: (NSError**)errorPtr;
-(NSData*)connect_request: (NSError**)errorPtr;

-(NSData*)wrap: (NSData*)message error(NSError**)errorPtr;
-(NSData*)unwrap: (NSData*)message error(NSError**)errorPtr;

-(void)send: (NSData*)message error(NSError**)errorPtr;
-(NSData*)recv: (NSError**)errorPtr;

@end
