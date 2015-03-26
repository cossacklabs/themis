/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import <Foundation/Foundation.h>
#import "objcthemis/ssession.h"
#import "objcthemis/error.h"

@interface Transport: SSession_transport_interface
{
  NSData* _client_pub_key;
}

-(instancetype)init;
-(NSData*) get_public_key: (NSData*)Id error:(NSError**)errorPtr;

@end

@implementation Transport

-(instancetype)init{
  self=[super init];
  if(self){
    _client_pub_key=[NSData dataWithContentsOfFile:@"client.pub"];
  }
  return self;
}

-(NSData*) get_public_key: (NSData*)Id error:(NSError**)errorPtr{
  if([Id isEqualToData:[NSData initWithBytes:"client"]]){
    return _client_pub_key;
  }
  *errorPtr=SCERROR(TErrorTypeFail, @"client Id not found in db");
  return NULL;
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
      NSString* id_str=@"server";
      NSData* id=[id_str dataUsingEncoding:NSUTF8StringEncoding];
      NSData* server_priv=[NSData dataWithContentsOfFile:@"server.priv"];
      Transport* transport=[[Transport alloc] init];
      SSession* session=[[SSession alloc]initWithId:id andPrivateKey:server_priv andCallbacks:transport];\


      NSString *londonWeatherUrl =  @"http://api.openweathermap.org/data/2.5/weather?q=London,uk";
      NSURLSession *session = [NSURLSession sharedSession];
      [[session dataTaskWithURL:[NSURL URLWithString:londonWeatherUrl]
	      completionHandler:^(NSData *data,
				  NSURLResponse *response,
				  NSError *error) {
            NSLog(@"data: @%", data);
	    
	  }] resume];
    }
    return 0;
}
