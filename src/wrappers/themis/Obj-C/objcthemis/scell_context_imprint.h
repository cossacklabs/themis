/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import <themis/themis.h>
#import "scell.h"

@interface SCell_context_imprint : SCell

- (id)initWithKey: (NSData*)key;
- (NSData*)wrapData: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr;
- (NSData*)unwrapData: (NSData*)message context:(NSData*)context error:(NSError**)errorPtr;

@end
