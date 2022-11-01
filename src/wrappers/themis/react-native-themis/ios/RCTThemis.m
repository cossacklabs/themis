//
//  RCTThemis.m
//
//  Created by Oleksii Radetskyi on 04.11.2021.
//

#import <Foundation/Foundation.h>
#import <React/RCTLog.h>
#import "RCTThemis.h"

@import themis;

@implementation RCTThemis

// To export a module named RCTThemis
RCT_EXPORT_MODULE(RCTThemis);

// To save the comparators objects
NSMutableDictionary* cmprs;

NSNumber *uchar_min;
NSNumber *uchar_max;

- (instancetype)init
{
  self = [super init];
  cmprs = [[NSMutableDictionary alloc] init];
  uchar_min = [NSNumber numberWithInt:0];
  uchar_max = [NSNumber numberWithInt:255];

  return self;
}

/* MARK: Export constants of the module */
- (NSDictionary *)constantsToExport
{
  return @{ @"COMPARATOR_NOT_READY": [[NSNumber alloc] initWithLong: TSComparatorNotReady],
            @"COMPARATOR_NOT_MATCH": [[NSNumber alloc] initWithLong: TSComparatorNotMatch],
            @"COMPARATOR_MATCH":     [[NSNumber alloc] initWithLong: TSComparatorMatch],
            @"COMPARATOR_ERROR":     @-1, // For compatibility with Java version
            @"KEYTYPE_RSA": @KEYTYPE_RSA,
            @"KEYTYPE_EC": @KEYTYPE_EC
 };
}

// It required by constantsToExport and iOS
+ (BOOL)requiresMainQueueSetup
{
  return YES;
}

/***********************************************/
/* Used to export NSData* to React Native app  */
/* NSData --> NSArray[NSNumber(unsigned char)] */
/*                                             */

+ (NSArray*)dataSerialize:(NSData*) data
{
  if (data == nil || data.length == 0 ) return nil;

  const char* buffer = (const char*) data.bytes;
  NSMutableArray<NSNumber*> *array = [[NSMutableArray alloc] init];

  for (NSInteger i = 0; i < data.length; i++) {
    NSNumber *num = [[NSNumber alloc] initWithUnsignedChar: buffer[i]];
    [array addObject:num];
  }
  return [array copy];
}

/*************************************************/
/* Used to import NSData* from React Native app  */
/* NSData <-- NSArray[NSNumber(unsigned char)]   */
/*                                               */

+ (NSData*)dataDeserialize: (NSArray*) data error:(NSError**) error
{
  if ( data == nil || data.count == 0 ) {
    *error = SCERROR(DESERIALIZE_ERROR, @DESERIALIZE_ERRORREASON);
    return nil;
  }

  char* buffer = (char*)malloc(data.count);
  if (buffer == nil) {
    *error = SCERROR(DESERIALIZE_MEMORY, @DESERIALIZE_MEMORYREASON);
    return nil;
  }

  for (NSInteger i = 0; i < data.count; i++) {
    NSNumber *num = data[i];
    /* Check int value before casting to char */
    if ([num compare:uchar_min] == NSOrderedAscending || [num compare:uchar_max] == NSOrderedDescending) {
      free(buffer); // did not forget to free allocated memory
      *error = SCERROR(BYTEOVERFLOW, @BYTEOVERFLOWREASON);
      return nil;
    }
    buffer[i] = num.unsignedCharValue;
  }
  NSData *result = [NSData dataWithBytesNoCopy:buffer length:data.count freeWhenDone:YES];
  return result;
}

RCT_EXPORT_METHOD(stringSerialize:(NSString*) text
                  callback:(RCTResponseSenderBlock)callback
                  )
{
  NSData   *data = [text dataUsingEncoding:NSUTF8StringEncoding];
  NSArray *data2 = [RCTThemis dataSerialize: data];
  callback(@[data2]);
}


RCT_EXPORT_METHOD(keyPair:(nonnull NSNumber*) algorithm
                  successCallback: (RCTResponseSenderBlock)successCallback
                  )
{
  TSKeyGen *keypair;
  switch (algorithm.intValue) {
    case KEYTYPE_RSA:
      keypair = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmRSA];
      break;
    default:
      keypair = [[TSKeyGen alloc] initWithAlgorithm:TSKeyGenAsymmetricAlgorithmEC];
      break;
  }

  NSArray  *privateKey = [RCTThemis dataSerialize: keypair.privateKey];
  NSArray   *publicKey = [RCTThemis dataSerialize: keypair.publicKey];

  NSDictionary *dictionary = @{
    @"private" : privateKey,
    @"public"  : publicKey
  };
  successCallback(@[dictionary]);
}


RCT_EXPORT_METHOD(symmetricKey:(RCTResponseSenderBlock)callback)
{
  NSData *symmetricKey = TSGenerateSymmetricKey();
  NSArray *masterKey = [RCTThemis dataSerialize: symmetricKey];
  callback(@[masterKey]);
}

- (TSCellSeal *)newSealMode: (NSArray*) symmetricKey error:(NSError**) error
{
  NSData *masterKey = [RCTThemis dataDeserialize: symmetricKey error:error];
  if (masterKey == nil) {
    return nil;
  }
  TSCellSeal *cell  = [[TSCellSeal alloc] initWithKey:masterKey];
  return cell;
}


RCT_EXPORT_METHOD(secureCellSealWithSymmetricKeyEncrypt:(NSArray*) symmetricKey
                  plaintext: (NSString*)plaintext
                  context: (NSString*)context
                  successCallback: (RCTResponseSenderBlock)successCallback
                  errorCallback: (RCTResponseErrorBlock)errorCallback)
{
  TSCellSeal *cell;
  NSError *error;

  cell = [self newSealMode:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }

  NSData *plaintextBinary = [plaintext dataUsingEncoding:NSUTF8StringEncoding];
  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *encrypted = [cell encrypt:plaintextBinary
                            context:contextBinary
                              error:&error];
  if (encrypted == nil) {
    errorCallback(error);
    return;
  }
  NSArray *result = [RCTThemis dataSerialize:encrypted];
  successCallback(@[result]);
}

RCT_EXPORT_METHOD(secureCellSealWithSymmetricKeyDecrypt:(NSArray*) symmetricKey
                  encrypted:(NSArray*) encrypted
                  context: (NSString*) context
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback
                  )

{
  TSCellSeal *cell;
  NSData *encryptedBinary;
  NSError *error;

  cell = [self newSealMode:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }

  encryptedBinary = [RCTThemis dataDeserialize:encrypted error:&error];
  if (encryptedBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *decrypted = [cell decrypt:encryptedBinary
                            context:contextBinary
                              error:&error];
  if (decrypted == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:decrypted];
    successCallback(@[result]);
  }
}


- (TSCellSeal *)newSealModeWithPassphrase: (NSString*) passphrase
{
  TSCellSeal *cell = [[TSCellSeal alloc] initWithPassphrase: passphrase];
  return cell;
}


RCT_EXPORT_METHOD(secureCellSealWithPassphraseEncrypt:(NSString*) passphrase
                  plaintext: (NSString*)plaintext
                  context: (NSString*)context
                  successCallback: (RCTResponseSenderBlock)successCallback
                  errorCallback: (RCTResponseErrorBlock)errorCallback
                  )
{
  TSCellSeal *cell = [self newSealModeWithPassphrase:passphrase];
  NSData *plaintextBinary = [plaintext dataUsingEncoding:NSUTF8StringEncoding];
  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];

  NSError *error;
  NSData *encrypted = [cell encrypt:plaintextBinary context:contextBinary error:&error];

  if (encrypted == nil) {
    errorCallback(error);
  } else {
    NSArray *result = [RCTThemis dataSerialize:encrypted];
    successCallback(@[result]);
  }
}

RCT_EXPORT_METHOD(secureCellSealWithPassphraseDecrypt:(NSString*) passphrase
                  encrypted:(NSArray*) encrypted
                  context: (NSString*) context
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback
                  )

{
  TSCellSeal *cell = [self newSealModeWithPassphrase:passphrase];
  NSData *encryptedBinary;
  NSError *error;

  encryptedBinary = [RCTThemis dataDeserialize:encrypted error:&error];
  if (encryptedBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *decrypted = [cell decrypt:encryptedBinary
                            context:contextBinary
                              error:&error];
  if (decrypted == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:decrypted];
    successCallback(@[result]);
  }
}

/* MARK: Token protect mode */

- (TSCellToken *)newTokenMode:(NSArray*) symmetricKey error:(NSError**) error
{
  NSData *masterKey = [RCTThemis dataDeserialize: symmetricKey error:error];
  if (masterKey == nil) {
    return nil;
  }

  TSCellToken *cell = [[TSCellToken alloc] initWithKey:masterKey];
  return cell;
}

RCT_EXPORT_METHOD(secureCellTokenProtectEncrypt:(NSArray*) symmetricKey
                  plaintext: (NSString*)plaintext
                  context: (NSString*)context
                  successCallback: (RCTResponseSenderBlock)successCallback
                  errorCallback: (RCTResponseErrorBlock)errorCallback)
{
  NSError *error;
  TSCellToken *cell;

  cell = [self newTokenMode:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }

  NSData *plaintextBinary = [plaintext dataUsingEncoding:NSUTF8StringEncoding];
  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];

  TSCellTokenEncryptedResult *result = [cell encrypt:plaintextBinary context:contextBinary error:&error];
  if (result == nil ) {
    errorCallback(error);
    return;
  }

  NSArray *encrypted = [RCTThemis dataSerialize: result.encrypted];
  NSArray *token     = [RCTThemis dataSerialize: result.token];

  NSDictionary *dictionary = @{
    @"encrypted" : encrypted,
    @"token"     : token
  };
  successCallback(@[dictionary]);
}

RCT_EXPORT_METHOD(secureCellTokenProtectDecrypt:(NSArray*) symmetricKey
                  encrypted:(NSArray*) encrypted
                  token:(NSArray*) token
                  context: (NSString*) context
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback
                  )

{
  TSCellToken *cell;
  NSData *encryptedBinary;
  NSData *tokenBinary;
  NSError *error;

  cell = [self newTokenMode:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }

  encryptedBinary = [RCTThemis dataDeserialize:encrypted error:&error];
  if (encryptedBinary == nil) {
    errorCallback(error);
    return;
  }

  tokenBinary = [RCTThemis dataDeserialize:token error:&error];
  if (tokenBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData *contextBinary = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *decrypted = [cell decrypt:encryptedBinary
                              token:tokenBinary
                            context:contextBinary
                              error:&error];
  if (decrypted == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:decrypted];
    successCallback(@[result]);
  }
}


/* MARK: Context imprint mode */

- (TSCellContextImprint *)newContextImprint:(NSArray*) symmetricKey error: (NSError**) error
{
  NSData *masterKey = [RCTThemis dataDeserialize: symmetricKey error:error];
  if (masterKey == nil) {
    return nil;
  }
  TSCellContextImprint *cell = [[TSCellContextImprint alloc] initWithKey:masterKey];
  return cell;
}

RCT_EXPORT_METHOD(secureCellContextImprintEncrypt:(NSArray*) symmetricKey
                  plaintext: (NSString*)plaintext
                  context: (NSString*)context
                  successCallback: (RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback)
{
  if (context == nil || [context isEqual: @""]) {
    NSError* error = SCERROR(CONTEXTREQUIRED, @CONTEXTREQUIREDREASON);
    errorCallback(error);
    return;
  }

  TSCellContextImprint *cell;
  NSError *error;

  cell = [self newContextImprint:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }

  NSData *plaintextBinary  = [plaintext dataUsingEncoding:NSUTF8StringEncoding];
  NSData *contextBinary  = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *encrypted = [cell encrypt:plaintextBinary context:contextBinary error:&error];
  if (encrypted == nil) {
    errorCallback(error);
    return;
  }

  NSArray *result = [RCTThemis dataSerialize:encrypted];
  successCallback(@[result]);
}

RCT_EXPORT_METHOD(secureCellContextImprintDecrypt:(NSArray*) symmetricKey
                  encrypted: (NSArray*) encrypted
                  context: (NSString*) context
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback)
{
  if (context == nil || [context isEqual: @""]) {
    NSError* error = SCERROR(CONTEXTREQUIRED, @CONTEXTREQUIREDREASON);
    errorCallback(error);
    return;
  }

  TSCellContextImprint *cell;
  NSData *encryptedBinary;
  NSError *error;

  cell = [self newContextImprint:symmetricKey error:&error];
  if (cell == nil) {
    errorCallback(error);
    return;
  }
  encryptedBinary = [RCTThemis dataDeserialize:encrypted error:&error];
  if (encryptedBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData *contextBinary  = [context dataUsingEncoding:NSUTF8StringEncoding];
  NSData *decrypted = [cell decrypt:encryptedBinary
                            context:contextBinary
                              error:&error
  ];

  if (decrypted == nil) {
    errorCallback(error);
    return;
  }

  NSArray* result = [RCTThemis dataSerialize:decrypted];
  successCallback(@[result]);
}

/* MARK: Secure Message */
RCT_EXPORT_METHOD(secureMessageSign:(NSString*) message
                  privateKey:(NSArray*) privateKey
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback
                  )
{
  if (privateKey == nil || privateKey.count == 0) {
    NSError* error = SCERROR(PRIVATEKEYREQUIRED, @PRIVATEKEYREQUIREDREASON);
    errorCallback(error);
    return;
  }

  NSData *privateKeyBinary;
  NSError *error;

  privateKeyBinary = [RCTThemis dataDeserialize:privateKey error:&error];
  if (privateKeyBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData* messageBinary = [message dataUsingEncoding:NSUTF8StringEncoding];
  TSMessage *secureMessage = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:privateKeyBinary
                                                                     peerPublicKey:nil];

  NSData *signedMessage = [secureMessage wrapData:messageBinary error:&error];
  if (signedMessage == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:signedMessage];
    successCallback(@[result]);
  }
}

RCT_EXPORT_METHOD(secureMessageVerify:(NSArray*) message
                  publicKey:(NSArray*) publicKey
                  successCallback:(RCTResponseSenderBlock)successCallback
                  errorCallback:(RCTResponseErrorBlock)errorCallback
                  )
{

  if (publicKey == nil || publicKey.count == 0) {
    NSError* error = SCERROR(PUBLICKEYREQUIRED, @PUBLICKEYREQUIREDREASON);
    errorCallback(error);
    return;
  }

  NSData  *publicKeyBinary;
  NSData  *messageBinary;
  NSError *error;

  publicKeyBinary = [RCTThemis dataDeserialize:publicKey error:&error];
  if (publicKeyBinary == nil) {
    errorCallback(error);
    return;
  }

  messageBinary = [RCTThemis dataDeserialize:message error:&error];
  if (messageBinary == nil) {
    errorCallback(error);
    return;
  }

  TSMessage *secureMessage = [[TSMessage alloc] initInSignVerifyModeWithPrivateKey:nil
                                                                     peerPublicKey:publicKeyBinary];

  NSData *verifiedMessage = [secureMessage unwrapData:messageBinary error:&error];
  if (verifiedMessage == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:verifiedMessage];
    successCallback(@[result]);
  }
}

RCT_EXPORT_METHOD(secureMessageEncrypt:(NSString*) message
                  privateKey:(NSArray*) privateKey
                  publicKey:(NSArray*) publicKey
                  successCallback:(RCTResponseSenderBlock) successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback
                  )

{
  if (privateKey == nil || privateKey.count == 0) {
    NSError* error = SCERROR(PRIVATEKEYREQUIRED, @PRIVATEKEYREQUIREDREASON);
    errorCallback(error);
    return;
  }

  if (publicKey == nil || publicKey.count == 0) {
      NSError* error = SCERROR(PUBLICKEYREQUIRED, @PUBLICKEYREQUIREDREASON);
      errorCallback(error);
      return;
  }

  NSData *privateKeyBinary;
  NSData *publicKeyBinary;
  NSError *error;

  privateKeyBinary = [RCTThemis dataDeserialize:privateKey error:&error];
  if (privateKeyBinary == nil) {
    errorCallback(error);
    return;
  }
  publicKeyBinary = [RCTThemis dataDeserialize:publicKey error:&error];
  if (publicKeyBinary == nil) {
    errorCallback(error);
    return;
  }

  NSData* messageBinary = [message dataUsingEncoding:NSUTF8StringEncoding];
  TSMessage *secureMessage = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyBinary
                                                                  peerPublicKey:publicKeyBinary];

  NSData *encryptedMessage = [secureMessage wrapData:messageBinary error:&error];
  if (encryptedMessage == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:encryptedMessage];
    successCallback(@[result]);
  }
}

RCT_EXPORT_METHOD(secureMessageDecrypt:(NSArray*) message
                  privateKey:(NSArray*) privateKey
                  publicKey:(NSArray*) publicKey
                  successCallback:(RCTResponseSenderBlock) successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback
                  )
{
  if (privateKey == nil || privateKey.count == 0) {
    NSError* error = SCERROR(PRIVATEKEYREQUIRED, @PRIVATEKEYREQUIREDREASON);
    errorCallback(error);
    return;
  }

  if (publicKey == nil || publicKey.count == 0) {
    NSError* error = SCERROR(PUBLICKEYREQUIRED, @PUBLICKEYREQUIREDREASON);
    errorCallback(error);
    return;
  }

  NSData *privateKeyBinary;
  NSData *publicKeyBinary;
  NSData *messageBinary;
  NSError *error;

  privateKeyBinary = [RCTThemis dataDeserialize:privateKey error:&error];
  if (privateKeyBinary == nil) {
    errorCallback(error);
    return;
  }
  publicKeyBinary = [RCTThemis dataDeserialize:publicKey error:&error];
  if (publicKeyBinary == nil) {
    errorCallback(error);
    return;
  }
  messageBinary = [RCTThemis dataDeserialize:message error:&error];
  if (messageBinary == nil) {
    errorCallback(error);
    return;
  }

  TSMessage *secureMessage = [[TSMessage alloc] initInEncryptModeWithPrivateKey:privateKeyBinary
                                                                  peerPublicKey:publicKeyBinary];

  NSData *decryptedMessage = [secureMessage unwrapData:messageBinary error:&error];
  if (decryptedMessage == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:decryptedMessage];
    successCallback(@[result]);
  }
}

/* MARK: Comparator */

RCT_EXPORT_METHOD(initComparator:(NSArray*) sharedSecret
                  successCallback:(RCTResponseSenderBlock) successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback)
{
  NSData *sharedSecretData;
  NSError *error;
  sharedSecretData = [RCTThemis dataDeserialize:sharedSecret error:&error];
  if (sharedSecretData == nil) {
    errorCallback(error);
    return;
  }

  TSComparator* cmp = [[TSComparator alloc] initWithMessageToCompare:sharedSecretData];
  NSString *uuid = [[NSUUID UUID] UUIDString];
  cmprs[uuid] = cmp;
  successCallback(@[uuid]);
}

RCT_EXPORT_METHOD(statusOfComparator:(NSString*) uuid
                  successCallback:(RCTResponseSenderBlock) successCallback
                  )
{
  TSComparator* cmp = cmprs[uuid];
  if (cmp == nil) {
    successCallback(@[@-1]);
  } else {
    NSNumber* status = [[NSNumber alloc] initWithInteger:(NSInteger)cmp.status];
    successCallback(@[status]);
  }
}

RCT_EXPORT_METHOD(beginCompare:(NSString*) uuid
                  successCallback:(RCTResponseSenderBlock) successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback
                  )
{
  TSComparator *cmp = cmprs[uuid];
  if (cmp == nil) {
    errorCallback(nil);
    return;
  }

  NSError *error;
  NSData *data = [cmp beginCompare:&error];
  if (data == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:data];
    successCallback(@[result]);
  }
}

RCT_EXPORT_METHOD(proceedCompare:(NSString*) uuid
                  previous:(NSArray*) previous
                  successCallback:(RCTResponseSenderBlock) successCallback
                  errorCallback:(RCTResponseErrorBlock) errorCallback
                  )
{
  TSComparator* cmp = cmprs[uuid];
  if ( cmp == nil ) {
    NSError* error = SCERROR(-1, @"Comparator does not exist");
    errorCallback(error);
    return;
  }

  NSData *data;
  NSError *error;

  data = [RCTThemis dataDeserialize:previous error:&error];
  if (data == nil) {
    errorCallback(error);
    return;
  }

  data = [cmp proceedCompare:data error:&error];
  if (data == nil) {
    errorCallback(error);
  } else {
    NSArray* result = [RCTThemis dataSerialize:data];
    NSNumber* status = [[NSNumber alloc] initWithInteger:(NSInteger)cmp.status];
    if (cmp.status != TSComparatorNotReady) {
      [cmprs removeObjectForKey:uuid];
    }
    if (result != nil) {
      successCallback(@[result, status]);
    } else {
      successCallback(@[@"", status]);
    }
  }
}


@end

