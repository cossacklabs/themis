/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>

@interface SKeygen : NSObject

{
  NSData* _priv_key;
  NSData* _pub_key;
}

- (id)init;
- (NSData*)priv_key: error:(NSError**)errorPtr;
- (NSData*)pub_key: error:(NSError**)errorPtr;

@end
