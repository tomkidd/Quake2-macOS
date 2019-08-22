//----------------------------------------------------------------------------------------------------------------------------
//
// "FDPreferences.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDPreferences : NSObject

- (instancetype) init UNAVAILABLE_ATTRIBUTE;

#if __has_feature(objc_class_property)
@property (class, readonly, retain) FDPreferences *sharedPrefs NS_SWIFT_NAME(shared);
#else
+ (FDPreferences*) sharedPrefs NS_SWIFT_NAME(shared);
#endif

- (void) registerDefaults: (NSDictionary<NSString*,id>*) dictionary;
- (void) registerDefaultObject: (NSObject*) object forKey: (NSString*) key;

- (void) setObject: (nullable id) object forKey: (NSString*) key;

- (BOOL) boolForKey: (NSString*) key;
- (NSInteger) integerForKey: (NSString*) key;
- (nullable NSString*) stringForKey: (NSString*) key;
- (nullable NSArray*) arrayForKey: (NSString*) key;

- (BOOL) synchronize;

@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
