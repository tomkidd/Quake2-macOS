//----------------------------------------------------------------------------------------------------------------------------
//
// "FDDisplay.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDisplayMode.h"

#import <Cocoa/Cocoa.h>

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDDisplay : NSObject

#if __has_feature(objc_class_property)
@property (class, readonly, copy) NSArray<FDDisplay*>* displays;
@property (class, readonly, retain) FDDisplay* mainDisplay;
#else
+ (NSArray<FDDisplay*>*) displays;
+ (FDDisplay*) mainDisplay;
#endif

@property (readonly) NSRect frame;

@property (readonly, copy) NSString *description;

@property (readonly, strong) FDDisplayMode* displayMode;
@property (readonly, strong) FDDisplayMode* originalMode;

@property (readonly, copy) NSArray<FDDisplayMode *> *displayModes;

- (BOOL) setDisplayMode: (FDDisplayMode*) displayMode;

@property (readonly, getter=isMainDisplay) BOOL mainDisplay;
@property (readonly, getter=isBuiltinDisplay) BOOL builtinDisplay;
@property (readonly, getter=isCaptured) BOOL captured;

@property (readonly) BOOL hasFSAA;

@property (readonly) float gamma;
- (void) setGamma: (float) gamma update: (BOOL) doUpdate;

- (void) fadeOutDisplay: (float) seconds;
- (void) fadeInDisplay: (float) seconds;

+ (void) fadeOutAllDisplays: (float) seconds;
+ (void) fadeInAllDisplays: (float) seconds;

- (void) captureDisplay;
- (void) releaseDisplay;

+ (void) captureAllDisplays;
+ (void) releaseAllDisplays;
#if __has_feature(objc_class_property)
@property (class, readonly, getter=isAnyDisplayCaptured) BOOL anyDisplayCaptured;
#else
+ (BOOL) isAnyDisplayCaptured;
#endif

@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
