//----------------------------------------------------------------------------------------------------------------------------
//
// "FDDisplayMode.m"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDisplayMode.h"
#import "FDDefines.h"

#import <Cocoa/Cocoa.h>
#include <ApplicationServices/ApplicationServices.h>

//----------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    CFStringRef     mName;
    NSUInteger      mBitsPerPixel;
} PixelEncodingToBitsPerPixel;

//----------------------------------------------------------------------------------------------------------------------------

static const PixelEncodingToBitsPerPixel   skPixelEncodingToBitsPerPixel[] = {
    { CFSTR (IO1BitIndexedPixels),  1 },
    { CFSTR (IO2BitIndexedPixels),  2 },
    { CFSTR (IO4BitIndexedPixels),  4 },
    { CFSTR (IO8BitIndexedPixels),  8 },
    { CFSTR (IO16BitDirectPixels), 16 },
    { CFSTR (IO32BitDirectPixels), 32 },
    { CFSTR(kIO30BitDirectPixels), 30 },
    { CFSTR(kIO64BitDirectPixels), 64 },
};

//----------------------------------------------------------------------------------------------------------------------------

@interface FDDisplayMode()

- (instancetype) initWithCGDisplayMode: (CGDisplayModeRef) mode;
@property (readonly, assign) CGDisplayModeRef cgDisplayMode;

@end

//----------------------------------------------------------------------------------------------------------------------------

@implementation FDDisplayMode
{
@private
    CGDisplayModeRef    mCGDisplayMode;
}

@synthesize cgDisplayMode = mCGDisplayMode;

- (instancetype) init
{
    self = [super init];
    
    if (self != nil) {
        [self doesNotRecognizeSelector: _cmd];
    }
    
    return nil;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) initWithCGDisplayMode: (CGDisplayModeRef) cgDisplayMode
{
    self = [super init];
    
    if (self)
    {
        const uint32_t  kMask   = kDisplayModeValidFlag | kDisplayModeNeverShowFlag | kDisplayModeNotGraphicsQualityFlag;
        BOOL            isValid = ((CGDisplayModeGetIOFlags (cgDisplayMode) & kMask) == kDisplayModeValidFlag);
        
        isValid = isValid && ((CGDisplayModeGetWidth (cgDisplayMode) * CGDisplayModeGetHeight (cgDisplayMode)) > 1);
        
        if (isValid) {
            mCGDisplayMode = CGDisplayModeRetain (cgDisplayMode);
        } else {
            return nil;
        }
    }
    
    return self;
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) dealloc
{
    if (mCGDisplayMode != NULL) {
        CGDisplayModeRelease (mCGDisplayMode);
    }
    
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSUInteger) width
{
    return CGDisplayModeGetWidth (mCGDisplayMode);
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSUInteger) height
{
    return CGDisplayModeGetHeight (mCGDisplayMode);
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) isStretched
{
    return (CGDisplayModeGetIOFlags (mCGDisplayMode) & kDisplayModeStretchedFlag) == kDisplayModeStretchedFlag;
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) isDefault
{
    return (CGDisplayModeGetIOFlags (mCGDisplayMode) & kDisplayModeDefaultFlag) == kDisplayModeDefaultFlag;
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSUInteger) bitsPerPixel
{
    CFStringRef pixelEncoding   = CGDisplayModeCopyPixelEncoding (mCGDisplayMode);
    NSUInteger  bitsPerPixel    = 0;
    
    for ( NSUInteger i = 0; i < FD_SIZE_OF_ARRAY (skPixelEncodingToBitsPerPixel); ++i )
    {
        if (CFStringCompare (pixelEncoding, skPixelEncodingToBitsPerPixel[i].mName, 0) == kCFCompareEqualTo)
        {
            bitsPerPixel = skPixelEncodingToBitsPerPixel[i].mBitsPerPixel;
            
            break;
        }
    }

    CFRelease (pixelEncoding);
    
    return bitsPerPixel;
}

//----------------------------------------------------------------------------------------------------------------------------

- (double) refreshRate
{
    return CGDisplayModeGetRefreshRate (mCGDisplayMode);
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSString*) description
{
    const unsigned long width           = self.width;
    const unsigned long height          = self.height;
    const double        refreshReate    = self.refreshRate;
    NSString*           description     = [NSString stringWithFormat: @"%lux%lu %.0fHz", width, height, refreshReate];
    
    if (self.stretched == YES)
    {
        description = [description stringByAppendingString: @" (stretched)"];
    }
    
    return description;
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) isEqualTo: (FDDisplayMode*) rhs
{
    if (!rhs)
    {
        return NO;
    }
    return (self.width == rhs.width) &&
           (self.height == rhs.height) &&
           (self.bitsPerPixel == rhs.bitsPerPixel) &&
           (self.stretched == rhs.stretched) &&
           (self.refreshRate == rhs.refreshRate);
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSComparisonResult) compare: (FDDisplayMode*) rhs
{
    if ([self isEqualTo:rhs]) {
        return NSOrderedSame;
    }

	const NSUInteger    lhsArea		= self.width * self.height;
	const NSUInteger    rhsArea     = rhs.width * rhs.height;
	NSComparisonResult  result		= NSOrderedDescending;
	
	if (lhsArea < rhsArea) {
		result = NSOrderedAscending;
	} else if (lhsArea == rhsArea) {
        if (self.refreshRate < rhs.refreshRate) {
            result = NSOrderedAscending;    
        } else if ((self.stretched == NO) && (rhs.stretched == YES)) {
            result = NSOrderedAscending;
        }
    }
    
	return result;
}

@end

//----------------------------------------------------------------------------------------------------------------------------
