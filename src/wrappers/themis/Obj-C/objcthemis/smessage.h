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
  NSData* priv_key_;
  NSData* peer_pub_key_;
}

- (id)init: (NSData*)private_key peer_pub_key:(NSData*)peer_pub_key;
- (NSData*)wrap: (NSData*)message;
- (NSData*)unwrap: (NSData*)message;

@end
