//----------------------------------------------------------------------------------------------------------------------------
//
// "FDLinkView.h" - Provides an URL style link button.
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>

//----------------------------------------------------------------------------------------------------------------------------

IB_DESIGNABLE
@interface FDLinkView : NSView

IBInspectable
@property (nonatomic, strong, nullable, setter=setURL:) NSURL *url;
IBInspectable
@property (nonatomic, copy, nullable) NSString *displayString;
- (void) setURL: (nullable NSURL*) url displayString: (nullable NSString*) displayString;

@end

//----------------------------------------------------------------------------------------------------------------------------
