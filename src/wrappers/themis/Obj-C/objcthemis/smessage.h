/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>

@interface SMessage : NSObject

{
  NSData* _priv_key;
  NSData* _peer_pub_key;
}

- (id)initWithPrivateKey: (NSData*)private_key peerPublicKey:(NSData*)peer_pub_key;
- (NSData*)wrap: (NSData*)message error:(NSError**)errorPtr;
- (NSData*)unwrap: (NSData*)message error:(NSError**)errorPtr;

@end
