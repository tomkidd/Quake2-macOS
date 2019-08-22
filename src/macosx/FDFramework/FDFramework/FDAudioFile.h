//----------------------------------------------------------------------------------------------------------------------------
//
// "FDAudioFile.h" - Sound file playback.
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDAudioMixer.h"

#import <Foundation/Foundation.h>

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDAudioFile : NSObject

@property (readonly, strong, nullable) NSURL *file;

- (instancetype) init NS_UNAVAILABLE;
- (nullable instancetype) initWithMixer: (nullable FDAudioMixer*) mixer NS_DESIGNATED_INITIALIZER;

@property float volume;

- (BOOL) startFile: (NSURL*) url loop: (BOOL) loop;
- (BOOL) stop;
- (BOOL) play;
- (BOOL) restart;

- (void) pause;
- (void) resume;

@property (readonly, getter=isPlaying) BOOL playing;
@property (readonly, getter=isFinished) BOOL finished;
@property (readonly) BOOL loops;

@end

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_END
