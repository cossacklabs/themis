/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>
#import "scell.h"

struct Encrypted_data{
  NSData* cipher_text;
  NSData* token;
};


@interface SCell_token : SCell

- (id)initWithKey: (NSData*)key;
- (struct Encrypted_data)wrap: (NSData*)message error:(NSError**)errorPtr;
- (NSData*)unwrap: (struct Encrypted_data)message error:(NSError**)errorPtr;
- (struct Encrypted_data)wrap: (NSData*)message context:(NSData*)contex error:(NSError**)errorPtr;
- (NSData*)unwrap: (struct Encrypted_data)message context:(NSData*)contex error:(NSError**)errorPtr;

@end
