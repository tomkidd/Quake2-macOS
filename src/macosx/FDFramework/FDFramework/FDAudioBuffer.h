//----------------------------------------------------------------------------------------------------------------------------
//
// "FDAudioBuffer.h" - Audio buffer playback.
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDAudioMixer.h"

#import <Foundation/Foundation.h>

//----------------------------------------------------------------------------------------------------------------------------

typedef NSUInteger (*FDAudioBufferCallback) (void* __null_unspecified pDst, NSUInteger numBytes, void* __null_unspecified pContext);

//----------------------------------------------------------------------------------------------------------------------------

@interface FDAudioBuffer : NSObject

- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
- (nullable instancetype) initWithMixer: (nullable FDAudioMixer*) mixer
                     frequency: (NSUInteger) frequency
                bitsPerChannel: (NSUInteger) bitsPerChannel
                      channels: (NSUInteger) channels
                      callback: (nullable FDAudioBufferCallback) pCallback
                       context: (nullable void*) pContext NS_DESIGNATED_INITIALIZER;

@property float volume;

@end

//----------------------------------------------------------------------------------------------------------------------------
