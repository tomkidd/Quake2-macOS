//----------------------------------------------------------------------------------------------------------------------------
//
// "FDAudioMixer.h" - Audio mixer.
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import <Foundation/Foundation.h>

//----------------------------------------------------------------------------------------------------------------------------

@interface FDAudioMixer : NSObject

#if __has_feature(objc_class_property)
@property (class, readonly, retain, nullable) FDAudioMixer *sharedAudioMixer;
#else
+ (nullable FDAudioMixer*) sharedAudioMixer;
#endif

- (nullable instancetype) init;

- (void) start;
- (void) stop;

@end

//----------------------------------------------------------------------------------------------------------------------------
