/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>
#import "scell.h"

@interface SCell_seal : SCell

- (id)initWithKey: (NSData*)key;
- (NSData*)wrap: (NSData*)message error:(NSError**)error;
- (NSData*)unwrap: (NSData*)message error:(NSError**)error;
- (NSData*)wrap: (NSData*)message context:(NSData*)contex  error:(NSError**)error;
- (NSData*)unwrap: (NSData*)message context:(NSData*)contex  error:(NSError**)error;

@end
