/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/21/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>

@interface SSession_transport_interface: NSObject

{
  secure_session_user_callbacks_t _callbacks;
}

-(instancetype)init;
-(void)send: (NSData*)data error:(NSError**)errorPtr;
-(NSData*) receive: (NSError**)errorPtr;
-(NSData*) get_public_key: (NSData*)Id error:(NSError**)errorPtr;
-(secure_session_user_callbacks_t*)callbacks;
@end
