//----------------------------------------------------------------------------------------------------------------------------
//
// "FDHIDManager.m"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDHIDManager.h"
#import "FDHIDInternal.h"
#import "FDDebug.h"
#import "FDDefines.h"
#import "FDPreferences.h"

#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hid/IOHIDLib.h>

//----------------------------------------------------------------------------------------------------------------------------

#define FD_HID_DEVICE_GAME_PAD      @"_FDHIDDeviceGamePad"
#define FD_HID_DEVICE_KEYBOARD      @"_FDHIDDeviceKeyboard"
#define FD_HID_DEVICE_MOUSE         @"_FDHIDDeviceMouse"

#define FD_HID_LCC_IDENTIFIER       @"com.Logitech.Control Center.Daemon"
#define FD_HID_LCC_SUPPRESS_WARNING @"LCCSuppressWarning"

//----------------------------------------------------------------------------------------------------------------------------

static NSString*        sDeviceFactories[]      = {
                                                    FD_HID_DEVICE_GAME_PAD,
                                                    FD_HID_DEVICE_KEYBOARD,
                                                    FD_HID_DEVICE_MOUSE
                                                  };

//----------------------------------------------------------------------------------------------------------------------------

NSString*const          FDHIDDeviceGamePad      = FD_HID_DEVICE_GAME_PAD;
NSString*const          FDHIDDeviceKeyboard     = FD_HID_DEVICE_KEYBOARD;
NSString*const          FDHIDDeviceMouse        = FD_HID_DEVICE_MOUSE;

//----------------------------------------------------------------------------------------------------------------------------

static FDHIDManager*    sFDHIDManagerInstance   = nil;

//----------------------------------------------------------------------------------------------------------------------------

static void             FDHIDManager_InputHandler (void*, IOReturn, void*, IOHIDValueRef);	
static void             FDHIDManager_DeviceMatchingCallback (void*, IOReturn, void*, IOHIDDeviceRef);
static void             FDHIDManager_DeviceRemovalCallback (void*, IOReturn, void*, IOHIDDeviceRef);

//----------------------------------------------------------------------------------------------------------------------------

@interface FDHIDManager ()

- (instancetype) initSharedHIDManager;
- (void) applicationWillResignActive: (NSNotification*) notification;
- (void) registerDevice: (IOHIDDeviceRef) pDevice;
- (void) unregisterDevice: (IOHIDDeviceRef) pDevice;

@end

//----------------------------------------------------------------------------------------------------------------------------

@implementation FDHIDManager
{
@private
    IOHIDManagerRef     mpIOHIDManager;
    NSMutableArray<FDHIDDevice*>*     mDevices;
    
    FDHIDEvent*         mpEvents;
    NSUInteger          mReadEvent;
    NSUInteger          mWriteEvent;
    NSUInteger          mMaxEvents;
}

//----------------------------------------------------------------------------------------------------------------------------

+ (FDHIDManager*) sharedHIDManager
{
    if (!sFDHIDManagerInstance)
    {
        sFDHIDManagerInstance = [[FDHIDManager alloc] initSharedHIDManager];
    }
    
    return sFDHIDManagerInstance;
}

//----------------------------------------------------------------------------------------------------------------------------

+ (void) checkForIncompatibleDevices
{
    [[FDPreferences sharedPrefs] registerDefaultObject: @NO forKey: FD_HID_LCC_SUPPRESS_WARNING];
    
    // check for Logitech Control Center. LCC installs its own kext and blocks HID events from Logitech devices
    if ([[NSWorkspace sharedWorkspace] absolutePathForAppBundleWithIdentifier: FD_HID_LCC_IDENTIFIER] != nil)
    {
        if ([[FDPreferences sharedPrefs] boolForKey: FD_HID_LCC_SUPPRESS_WARNING] == NO)
        {
            NSAlert*    alert   = [[[NSAlert alloc] init] autorelease];
            NSString*   appName = [NSRunningApplication currentApplication].localizedName;
            NSString*   message = [NSString stringWithFormat: @"An installation of the Logitech Control Center software "
                                   @"has been detected. This software is not compatible with %@.",
                                   appName];
            NSString*   informative = [NSString stringWithFormat: @"Please uninstall the Logitech Control Center software "
                                       @"if you want to use a Logitech input device with %@.",
                                       appName];
            
            alert.messageText = message;
            alert.informativeText = informative;
            alert.alertStyle = NSCriticalAlertStyle;
            [alert setShowsSuppressionButton: YES];
            [alert runModal];
            
            [[FDPreferences sharedPrefs] setObject: alert.suppressionButton forKey: FD_HID_LCC_SUPPRESS_WARNING];
        }
    }
    else
    {
        // reset the warning in case LCC was uninstalled
        [[FDPreferences sharedPrefs] setObject: @NO forKey: FD_HID_LCC_SUPPRESS_WARNING];
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) init
{
    self = [super init];
    
    if (self != nil)
    {
        [self doesNotRecognizeSelector: _cmd];
        [self release];
    }
    
    return nil;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) initSharedHIDManager
{
    self = [super init];
    
    if (self != nil)
    {
        BOOL success = YES;
        
        if (success)
        {
            mpIOHIDManager  = IOHIDManagerCreate (kCFAllocatorDefault, kIOHIDManagerOptionNone);
            success         = (mpIOHIDManager != NULL);
        }
        
        if (success)
        {
            success = (IOHIDManagerOpen (mpIOHIDManager, kIOHIDManagerOptionNone) == kIOReturnSuccess);
        }
        
        if (success)
        {
            mDevices = [[NSMutableArray alloc] initWithCapacity: 3];
            success  = (mDevices != nil);
        }
        
        if (success)
        {
            NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];
            
            [notificationCenter addObserver: self
                                   selector: @selector (applicationWillResignActive:)
                                       name: NSApplicationWillResignActiveNotification 
                                     object: nil];
        }
        
        if (!success)
        {
            [self release];
            return nil;
        }
    }
    
    return self;    
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver: self];
    [mDevices release];
    
    if (mpIOHIDManager)
    {
        IOHIDManagerRegisterDeviceMatchingCallback (mpIOHIDManager, NULL, NULL);
        IOHIDManagerRegisterDeviceRemovalCallback (mpIOHIDManager, NULL, NULL);
        IOHIDManagerUnscheduleFromRunLoop (mpIOHIDManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        IOHIDManagerClose (mpIOHIDManager, kIOHIDManagerOptionNone);
    }
    
    if (mpEvents)
    {
        free (mpEvents);
    }
    
    sFDHIDManagerInstance = nil;
    
    [super dealloc];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) applicationWillResignActive: (NSNotification*) notification
{
    FD_UNUSED (notification);
    
    for (FDHIDDevice* device in mDevices)
    {
        [device flush];
    }
    
    mReadEvent  = 0;
    mWriteEvent = 0;
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) setDeviceFilter: (NSArray*) devices
{
    NSMutableArray* matchingArray = nil;
    
    if (devices != nil)
    {
        matchingArray = [NSMutableArray array];

        for (NSString* deviceName in devices)
        {
            Class   device = NSClassFromString (deviceName);
            
            if (device != nil)
            {
                NSArray* dicts = [device matchingDictionaries];
                
                if (dicts != nil)
                {
                    [matchingArray addObjectsFromArray: dicts];
                }
            }
        }
            
        if (matchingArray.count == 0)
        {
            matchingArray = nil;
        }
     }
    
    IOHIDManagerSetDeviceMatchingMultiple (mpIOHIDManager, (CFMutableArrayRef) matchingArray);
    IOHIDManagerRegisterDeviceMatchingCallback (mpIOHIDManager, FDHIDManager_DeviceMatchingCallback, self);
    IOHIDManagerRegisterDeviceRemovalCallback (mpIOHIDManager, FDHIDManager_DeviceRemovalCallback, self);
    IOHIDManagerScheduleWithRunLoop (mpIOHIDManager, CFRunLoopGetCurrent (), kCFRunLoopDefaultMode);
}

//----------------------------------------------------------------------------------------------------------------------------

- (NSArray*) devices
{
    return mDevices;
}

//----------------------------------------------------------------------------------------------------------------------------

- (const FDHIDEvent*) nextEvent
{
    const FDHIDEvent* pEvent = nil;
     
    if (mReadEvent < mWriteEvent)
    {
        pEvent = &(mpEvents[mReadEvent++]);
    }
    else
    {
        mReadEvent  = 0;
        mWriteEvent = 0;
    }
     
    return pEvent;
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) pushEvent: (const FDHIDEvent*) pEvent
{
    if (NSApp.active == YES)
    {
        if (mWriteEvent == mMaxEvents)
        {
            mMaxEvents   = (mMaxEvents + 1) << 1;
            mpEvents     = realloc (mpEvents, mMaxEvents * sizeof (FDHIDEvent));
            
            if (mpEvents == NULL)
            {
                mReadEvent  = 0;
                mWriteEvent = 0;
                mMaxEvents  = 0;
            }
        }
        
        if (mWriteEvent < mMaxEvents)
        {
            mpEvents[mWriteEvent++] = *pEvent;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) registerDevice: (IOHIDDeviceRef) pDevice
{
    for (NSUInteger i = 0; i < FD_SIZE_OF_ARRAY (sDeviceFactories); ++i)
    {
        Class factory = NSClassFromString (sDeviceFactories[i]);
        
        if (factory != nil)
        {
            FD_ASSERT ([factory isSubclassOfClass: [FDHIDDevice class]]);
            FD_ASSERT ([factory respondsToSelector: @selector (deviceWithDevice:)]);
            
            FDHIDDevice *device = [factory deviceWithDevice: pDevice];
            
            if (device != nil)
            {
                device.delegate = self;
                [mDevices addObject: device];
                
                IOHIDDeviceRegisterInputValueCallback (pDevice, &FDHIDManager_InputHandler, device);
                
                break;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) unregisterDevice: (IOHIDDeviceRef) pDevice
{
    IOHIDDeviceRegisterInputValueCallback (pDevice, NULL, NULL);
 
    for (FDHIDDevice* device in mDevices)
    {
        if (device.iohidDeviceRef == pDevice)
        {
            [mDevices removeObject: device];
            break;
        }
    }
}

@end

//----------------------------------------------------------------------------------------------------------------------------

void FDHIDManager_InputHandler (void* pContext, IOReturn result, void* pSender, IOHIDValueRef pValue)
{
    FD_UNUSED (result, pSender);
    FD_ASSERT (pContext != nil);
    FD_ASSERT (pValue != nil);
    
    if (NSApp.active == YES)
    {
        FDHIDDevice* device = (FDHIDDevice*) pContext;
        
        [device handleInput: pValue];
    }
}

//----------------------------------------------------------------------------------------------------------------------------

void FDHIDManager_DeviceMatchingCallback (void* pContext, IOReturn result, void* pSender, IOHIDDeviceRef pDevice)
{
    FD_UNUSED (result, pSender);
    FD_ASSERT (pContext == sFDHIDManagerInstance);
    FD_ASSERT (pDevice != nil);

    [((FDHIDManager*) pContext) registerDevice: pDevice];
}

//----------------------------------------------------------------------------------------------------------------------------

void FDHIDManager_DeviceRemovalCallback (void* pContext, IOReturn result, void* pSender, IOHIDDeviceRef pDevice) 
{
    FD_UNUSED (result, pSender);
    FD_ASSERT (pContext == sFDHIDManagerInstance);
    FD_ASSERT (pDevice != nil);
    
    [((FDHIDManager*) pContext) unregisterDevice: pDevice];
}

//----------------------------------------------------------------------------------------------------------------------------
