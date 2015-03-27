/**
 * @file
 *
 * Created by Andrey Mnatsakanov on 03/18/2015
 * (c) CossackLabs
 */

typedef enum{
  TErrorTypeSuccess=0,
  TErrorTypeBufferTooSmall=-4,
  TErrorTypeFail=-1,
  TErrorTypeSendAsIs=1
} TErrorType;

#define SCERROR(error_code, error_message)				\
  [NSError errorWithDomain:@"com.CossackLabs.Themis.ErrorDomain"	\
  code:error_code \
  userInfo:[[NSDictionary alloc] initWithObjectsAndKeys:error_message, @"NSLocalizedDescriptionKey",NULL]]
