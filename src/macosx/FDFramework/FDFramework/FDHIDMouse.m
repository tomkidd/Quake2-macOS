//----------------------------------------------------------------------------------------------------------------------------
//
// "FDHIDMouse.m"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDHIDManager.h"
#import "FDHIDInternal.h"
#import "FDDebug.h"
#import "FDDefines.h"

#import <Cocoa/Cocoa.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hid/IOHIDLib.h>

//----------------------------------------------------------------------------------------------------------------------------

static void     FDHIDMouse_AxisHandler (FDHIDDevice*, unsigned int, IOHIDValueRef, IOHIDElementRef);
static void     FDHIDMouse_ButtonHandler (FDHIDDevice*, unsigned int, IOHIDValueRef, IOHIDElementRef);

//----------------------------------------------------------------------------------------------------------------------------

static FDHIDButtonMap   sFDHIDMouseDefaultAxisMap[] = 
{
    { kHIDUsage_GD_X,           FDHIDMouseAxisX,            &FDHIDMouse_AxisHandler             },
    { kHIDUsage_GD_Y,           FDHIDMouseAxisY,            &FDHIDMouse_AxisHandler             },
    { kHIDUsage_GD_Z,           0,                          NULL                                },
    { kHIDUsage_GD_Rx,          0,                          NULL                                },
    { kHIDUsage_GD_Ry,          0,                          NULL                                },
    { kHIDUsage_GD_Rz,          0,                          NULL                                },
    { kHIDUsage_GD_Slider,      0,                          NULL                                },
    { kHIDUsage_GD_Dial,        0,                          NULL                                },
    { kHIDUsage_GD_Wheel,       FDHIDMouseAxisWheel,        &FDHIDMouse_AxisHandler             }
};

//----------------------------------------------------------------------------------------------------------------------------

static FDHIDButtonMap   sFDHIDMouseDefaultButtonMap[] = 
{
    { kHIDUsage_Button_1 + 0,   0,                          &FDHIDMouse_ButtonHandler           }, 
    { kHIDUsage_Button_1 + 1,   1,                          &FDHIDMouse_ButtonHandler           }, 
    { kHIDUsage_Button_1 + 2,   2,                          &FDHIDMouse_ButtonHandler           }, 
    { kHIDUsage_Button_1 + 3,   3,                          &FDHIDMouse_ButtonHandler           },
    { kHIDUsage_Button_1 + 4,   4,                          &FDHIDMouse_ButtonHandler           }, 
};

//----------------------------------------------------------------------------------------------------------------------------

static FDHIDElementMap  sFDHIDMouseDefaultMap[] =
{
    { kIOHIDElementTypeInput_Misc,   FD_SIZE_OF_ARRAY (sFDHIDMouseDefaultAxisMap),   &(sFDHIDMouseDefaultAxisMap[0])    },
    { kIOHIDElementTypeInput_Button, FD_SIZE_OF_ARRAY (sFDHIDMouseDefaultButtonMap), &(sFDHIDMouseDefaultButtonMap[0])  }
};

//----------------------------------------------------------------------------------------------------------------------------

static FDHIDDeviceDesc  sFDHIDMouseMap[] =
{
    { -1, -1, &(sFDHIDMouseDefaultMap[0]), FD_SIZE_OF_ARRAY (sFDHIDMouseDefaultMap), 0 }
};

//----------------------------------------------------------------------------------------------------------------------------

FDHIDUsageToDevice gFDHIDMouseUsageMap[] =
{
    { kHIDPage_GenericDesktop,  kHIDUsage_GD_Mouse,     &(sFDHIDMouseMap[0]), FD_SIZE_OF_ARRAY (sFDHIDMouseMap), 0 },
    { kHIDPage_Consumer,        kHIDUsage_GD_Pointer,   &(sFDHIDMouseMap[0]), FD_SIZE_OF_ARRAY (sFDHIDMouseMap), 0 }
};

//----------------------------------------------------------------------------------------------------------------------------

@interface _FDHIDDeviceMouse : FDHIDDevice

+ (NSArray*) matchingDictionaries;
+ (FDHIDDevice*) deviceWithDevice: (IOHIDDeviceRef) pDevice;

@end

//----------------------------------------------------------------------------------------------------------------------------

@implementation _FDHIDDeviceMouse

+ (NSArray*) matchingDictionaries
{
    return [self matchingDictionaries: gFDHIDMouseUsageMap withCount: FD_SIZE_OF_ARRAY (gFDHIDMouseUsageMap)]; 
}

//----------------------------------------------------------------------------------------------------------------------------

+ (FDHIDDevice*) deviceWithDevice: (IOHIDDeviceRef) pDevice
{
    const NSUInteger            numUsages   = FD_SIZE_OF_ARRAY (gFDHIDMouseUsageMap);
    const FDHIDUsageToDevice*   pUsageMap   = &(gFDHIDMouseUsageMap[0]);
    
    return [self deviceWithDevice: pDevice usageMap: pUsageMap count: numUsages];
}

@end

//----------------------------------------------------------------------------------------------------------------------------

void FDHIDMouse_AxisHandler (FDHIDDevice *device, unsigned int axis, IOHIDValueRef pValue, IOHIDElementRef pElement)
{
    FD_UNUSED (pElement);
    FD_ASSERT (pValue != nil);
    
    FDHIDEvent event = { 0 };
    
    event.mDevice   = device;
    event.mType     = FDHIDEventTypeMouseAxis;
    event.mButton   = axis;
    event.mIntVal   = (signed int) IOHIDValueGetIntegerValue (pValue);
    
    [device pushEvent: &event];
}

//----------------------------------------------------------------------------------------------------------------------------

void FDHIDMouse_ButtonHandler (FDHIDDevice *device, unsigned int button, IOHIDValueRef pValue, IOHIDElementRef pElement)
{
    FD_UNUSED (pElement);
    FD_ASSERT (pValue != nil);
    
    FDHIDEvent      event = { 0 };
    
    event.mDevice   = device;
    event.mType     = FDHIDEventTypeMouseButton;
    event.mButton   = button;
    event.mIntVal   = (IOHIDValueGetIntegerValue (pValue) != 0);
    
    [device pushEvent: &event];
}

//----------------------------------------------------------------------------------------------------------------------------
