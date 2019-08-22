//----------------------------------------------------------------------------------------------------------------------------
//
// "FDWindow.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDisplay.h"

#import <Cocoa/Cocoa.h>

#ifndef NS_SWIFT_NAME
#define NS_SWIFT_NAME(...)
#endif

@class FDView;

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------------

typedef void (*FDResizeHandler) (FDView *fdView, void*__nullable pContext);

//----------------------------------------------------------------------------------------------------------------------------

@interface FDWindow : NSWindow

- (instancetype) initForDisplay: (FDDisplay*) display NS_SWIFT_NAME(init(display:));
- (instancetype) initForDisplay: (FDDisplay*) display samples: (NSUInteger) samples NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(display:samples:));

- (instancetype) initWithContentRect: (NSRect) rect;
- (instancetype) initWithContentRect: (NSRect) rect samples: (NSUInteger) samples NS_DESIGNATED_INITIALIZER;
- (instancetype) initWithContentRect: (NSRect) contentRect styleMask: (NSWindowStyleMask) style backing: (NSBackingStoreType) backingStoreType defer: (BOOL) flag UNAVAILABLE_ATTRIBUTE;

- (void) setResizeHandler: (nullable FDResizeHandler) pResizeHandler forContext: (nullable void*) pContext;

- (void) centerForDisplay: (FDDisplay*) display;

@property (nonatomic, getter=isCursorVisible) BOOL cursorVisible;
@property BOOL vsync;

@property (readonly, strong) NSOpenGLContext *openGLContext;

@property (readonly, getter=isFullscreen) BOOL fullscreen;

- (void) endFrame;

@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
