//----------------------------------------------------------------------------------------------------------------------------
//
// "FDDisplayMode.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDDisplayMode : NSObject

@property (readonly) NSUInteger width;
@property (readonly) NSUInteger height;
@property (readonly) NSUInteger bitsPerPixel;
@property (readonly, getter=isStretched) BOOL stretched;
@property (readonly, getter=isDefault) BOOL isDefault;
@property (readonly) double refreshRate;

@property (readonly, copy) NSString *description;

- (BOOL) isEqualTo: (nullable FDDisplayMode*) object;
- (NSComparisonResult) compare: (FDDisplayMode*) rhs;

- (instancetype) init UNAVAILABLE_ATTRIBUTE;


@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
