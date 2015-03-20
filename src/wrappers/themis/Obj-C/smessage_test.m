/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "objcthemis/smessage.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        @try {
	  NSData *server_priv=[NSData dataWithContentsOfFile:@"server.priv"];
	  NSData *server_pub=[NSData dataWithContentsOfFile:@"server.pub"];
	  NSData *client_priv=[NSData dataWithContentsOfFile:@"client.priv"];
	  NSData *client_pub=[NSData dataWithContentsOfFile:@"client.pub"];

	  SMessage* encrypter=[[SMessage alloc]init:client_priv peer_pub_key:server_pub]; 
	  SMessage* decrypter=[[SMessage alloc]init:server_priv peer_pub_key:client_pub];

	  NSData* encrypted_message=[encrypter wrap:[NSKeyedArchiver archivedDataWithRootObject:@"This is test message"]];
	  NSLog(@"%@",encrypted_message);
	  NSData* decrypted_message=[decrypter unwrap:encrypted_message];
	  NSLog([NSKeyedUnarchiver unarchiveObjectWithData:decrypted_message]);
        } @catch(NSException *theException) {
	  if ([theException.name isEqual:@"ThemisException"]) {
                NSLog(@"Caught an EmptyInventoryException");
            } else {
                NSLog(@"Ignored a %@ exception", theException);
                @throw;
            }
        }
    }
    return 0;
}
