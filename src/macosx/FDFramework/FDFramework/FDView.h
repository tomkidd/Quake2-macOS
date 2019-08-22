//----------------------------------------------------------------------------------------------------------------------------
//
// "FDView.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDView : NSView

- (instancetype) initWithFrame: (NSRect) frameRect;

@property (nonatomic, strong, null_resettable) NSCursor *cursor;
@property BOOL vsync;

@property (readonly, nonatomic, strong) NSOpenGLContext *openGLContext;

@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
