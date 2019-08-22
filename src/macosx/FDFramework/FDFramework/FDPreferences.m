//----------------------------------------------------------------------------------------------------------------------------
//
// "FDPreferences.m"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDPreferences.h"
#import "FDDebug.h"
#import "FDDisplay.h"
#import "FDDisplayMode.h"

//----------------------------------------------------------------------------------------------------------------------------

static FDPreferences*   sFDPreferencesInstance  = nil;

//----------------------------------------------------------------------------------------------------------------------------

@interface FDPreferences ()

- (instancetype) initSharedPreferences NS_DESIGNATED_INITIALIZER;

@end

//----------------------------------------------------------------------------------------------------------------------------

@implementation FDPreferences

//----------------------------------------------------------------------------------------------------------------------------

+ (FDPreferences*) sharedPrefs
{
    if (!sFDPreferencesInstance)
    {
        sFDPreferencesInstance = [[self alloc] initSharedPreferences];
    }
    
    return sFDPreferencesInstance;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) init
{
    self = [self initSharedPreferences];
    
    if (self != nil)
    {
        [self doesNotRecognizeSelector: _cmd];
    }
    
    return nil;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) initSharedPreferences
{
    self = [super init];
    
    return self;
}

//----------------------------------------------------------------------------------------------------------------------------

- (id) serializableFromObject: (id) object
{
    id serializable = nil;
    
    if ([object isKindOfClass: [FDDisplay class]] == YES)
    {
        serializable = [object description];
    }
    else if ([object isKindOfClass: [FDDisplayMode class]] == YES)
    {
        serializable = [object description];
    }
    else if ([object isKindOfClass: [NSTextField class]] == YES)
    {
        serializable = [object stringValue];
    }
    else if ([object isKindOfClass: [NSPopUpButton class]] == YES)
    {
        serializable = [self serializableFromObject: [object selectedItem].representedObject];
        
        if (serializable == nil)
        {
            serializable = @([object selectedTag]);
        }
    }
    else if ([object isKindOfClass: [NSButton class]] == YES)
    {
        serializable = @((BOOL)([object state] == NSOnState));
    }
    else if ([object isKindOfClass: [NSString class]] == YES)
    {
        serializable = object;
    }
    else if ([object isKindOfClass: [NSNumber class]] == YES)
    {
        serializable = object;
    }
    else if ([object isKindOfClass: [NSArray class]] == YES)
    {
        serializable = object;
    }
    else if ([object isKindOfClass: [NSDictionary class]] == YES)
    {
        serializable = object;
    }
    
    return serializable;
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) registerDefaults: (NSDictionary*) dictionary
{
    [[NSUserDefaults standardUserDefaults] registerDefaults: dictionary];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) registerDefaultObject: (NSObject*) object forKey: (NSString*) key
{
    [self registerDefaults: @{key: [self serializableFromObject: object]}];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) setObject: (id) object forKey: (NSString*) key
{
    id serializable = [self serializableFromObject: object];
    
    if (serializable != nil)
    {
        [[NSUserDefaults standardUserDefaults] setObject: serializable forKey: key];
    }
    else
    {
        FDLog (@"FDPreferences: cannot serialize class of type: %@!", [object class]);
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) boolForKey: (NSString*) key
{
    return [[NSUserDefaults standardUserDefaults] boolForKey: key];
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSInteger) integerForKey: (NSString*) key
{
    return [[NSUserDefaults standardUserDefaults] integerForKey: key];
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSString*) stringForKey: (NSString*) key
{
    return [[NSUserDefaults standardUserDefaults] stringForKey: key];
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSArray*) arrayForKey: (NSString*) key
{   
    return [[NSUserDefaults standardUserDefaults] arrayForKey: key];
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) synchronize
{
    return [[NSUserDefaults standardUserDefaults] synchronize];
}

@end

//---------------------------------------------------------------------------------------------------------------------------
