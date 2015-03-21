/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>

@interface SCell : NSObject
{
  NSData* _key;
}

-(instancetype)initWithKey: (NSData*)key;

@end
