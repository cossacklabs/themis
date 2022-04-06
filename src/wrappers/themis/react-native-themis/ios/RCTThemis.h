//
//  RCTThemis.h
//  ThemisExample
//
//  Created by Oleksii Radetskyi on 04.11.2021.
//

#ifndef RCTThemis_h
#define RCTThemis_h

#import <React/RCTBridgeModule.h>

#define BYTEOVERFLOW        255
#define CONTEXTREQUIRED     254
#define PUBLICKEYREQUIRED   253
#define PRIVATEKEYREQUIRED  252

#define CONTEXTREQUIREDREASON "Context required"
#define PUBLICKEYREQUIREDREASON "Public key can not be null or blank"
#define PRIVATEKEYREQUIREDREASON "Private key can not be null or blank"

#define KEYTYPE_EC  0
#define KEYTYPE_RSA 1


@interface RCTThemis : NSObject <RCTBridgeModule>
@end

#endif /* RCTThemis_h */
