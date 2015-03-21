/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

#import "objcthemis/smessage.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
	  NSData *server_priv=[NSData dataWithContentsOfFile:@"server.priv"];
	  NSData *server_pub=[NSData dataWithContentsOfFile:@"server.pub"];
	  NSData *client_priv=[NSData dataWithContentsOfFile:@"client.priv"];
	  NSData *client_pub=[NSData dataWithContentsOfFile:@"client.pub"];

	  SMessage* encrypter=[[SMessage alloc]initWithPrivateKey:client_priv peerPublicKey:server_pub]; 
	  SMessage* decrypter=[[SMessage alloc]initWithPrivateKey:server_priv peerPublicKey:client_pub];

	  NSError *themis_error=NULL;
	  NSData* encrypted_message=[encrypter wrap:[NSKeyedArchiver archivedDataWithRootObject:@"This is test message"] error:&themis_error];
	  if(themis_error){
              NSLog(@"Error ocured %@", themis_error);
	      return -1;
	  }
	  NSLog(@"%@",encrypted_message);
	  NSData* decrypted_message=[decrypter unwrap:encrypted_message error:&themis_error];
	  if(themis_error){
              NSLog(@"Error ocured %@", themis_error);
	      return -2;
	  }
	  NSLog([NSKeyedUnarchiver unarchiveObjectWithData:decrypted_message]);
    }
    return 0;
}
