/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>

struct Encrypted_data{
  NSData* cipher_text;
  NSData* token;
};


@interface SCell_token : NSObject
{
  NSData* key_;
}

- (id)init: (NSData*)key;
- (struct Encrypted_data)wrap: (NSData*)message;
- (NSData*)unwrap: (struct Encrypted_data)message;
- (struct Encrypted_data)wrap: (NSData*)message context:(NSData*)contex;
- (NSData*)unwrap: (struct Encrypted_data)message context:(NSData*)contex;

@end
