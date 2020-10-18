//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//


#ifndef objcthemis_Bridging_Header_h
#define objcthemis_Bridging_Header_h

    #import "StaticKeys.h"

// TODO: use a unified import here
// CocoaPods tests do not work with canonical import of Themis for some reason.
// Please fix this if you have any idea how.
#if COCOAPODS
#import <objcthemis/objcthemis.h>
#else
@import themis;
#endif

#endif /* objcthemis_Bridging_Header_h */
