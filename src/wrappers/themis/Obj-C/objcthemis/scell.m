/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "scell.h"

@implementation SCell

-(instancetype)initWithKey: (NSData*)key{
  self=[super init];
  if (self){
    _key=[[NSData alloc]initWithData:key];
  }
  return self;
}

@end
