/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/21/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>

@interface SSession_transport_intergace: NSObject

-(void)send: (NSData*)data error(NSError**)errorPtr;
-(NSDtata*) receive: (NSError**)errorPtr;
-(NSDtata*) get_public_key: (NSData*)Id;

@end
