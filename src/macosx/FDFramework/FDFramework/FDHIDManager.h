//----------------------------------------------------------------------------------------------------------------------------
//
// "FDHIDManager.h" - HID input
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDefines.h"
#import "FDHIDDevice.h"

#import <Cocoa/Cocoa.h>
#include <IOKit/hid/IOHIDLib.h>

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------------

FD_EXTERN NSString*const    FDHIDDeviceGamePad;
FD_EXTERN NSString*const    FDHIDDeviceKeyboard;
FD_EXTERN NSString*const    FDHIDDeviceMouse;

//----------------------------------------------------------------------------------------------------------------------------

typedef NS_ENUM(int, FDHIDEventType)
{
    FDHIDEventTypeGamePadAxis,
    FDHIDEventTypeGamePadButton,
    FDHIDEventTypeKeyboard,
    FDHIDEventTypeMouseAxis,
    FDHIDEventTypeMouseButton
};

//----------------------------------------------------------------------------------------------------------------------------

typedef NS_ENUM(int, FDHIDKey)
{
    FDHIDKeyTab             = 9,
    FDHIDKeyEnter           = 13,
    FDHIDKeyEscape          = 27,
    FDHIDKeySpace           = 32,
    FDHIDKeyBackspace       = 127,
    FDHIDKeyUpArrow         = 128,
    FDHIDKeyDownArrow       = 129,
    FDHIDKeyLeftArrow       = 130,
    FDHIDKeyRightArrow      = 131,
    FDHIDKeyAlternate       = 132,
    FDHIDKeyOption          = 132,
    FDHIDKeyControl         = 133,
    FDHIDKeyShift           = 134,
    FDHIDKeyF1              = 135,
    FDHIDKeyF2              = 136,
    FDHIDKeyF3              = 137,
    FDHIDKeyF4              = 138,
    FDHIDKeyF5              = 139,
    FDHIDKeyF6              = 140,
    FDHIDKeyF7              = 141,
    FDHIDKeyF8              = 142,
    FDHIDKeyF9              = 143,
    FDHIDKeyF10             = 144,
    FDHIDKeyF11             = 145,
    FDHIDKeyF12             = 146,
    FDHIDKeyInsert          = 147,
    FDHIDKeyDelete          = 148,
    FDHIDKeyPageDown        = 149,
    FDHIDKeyPageUp          = 150,
    FDHIDKeyHome            = 151,
    FDHIDKeyEnd             = 152,
    FDHIDKeyCapsLock        = 153,
    FDHIDKeyCommand         = 154,
    FDHIDKeyNumLock         = 155,
    FDHIDKeyF13             = 156,
    FDHIDKeyF14             = 157,
    FDHIDKeyF15             = 158,
    FDHIDKeyF16             = 159,
    FDHIDKeyPadEqual        = 160,
    FDHIDKeyPadSlash        = 161,
    FDHIDKeyPadAsterisk     = 162,
    FDHIDKeyPadMinus        = 163,
    FDHIDKeyPadPlus         = 164,
    FDHIDKeyPadEnter        = 165,
    FDHIDKeyPadPeriod       = 166,
    FDHIDKeyPad0            = 167,
    FDHIDKeyPad1            = 168,
    FDHIDKeyPad2            = 169,
    FDHIDKeyPad3            = 170,
    FDHIDKeyPad4            = 171,
    FDHIDKeyPad5            = 172,
    FDHIDKeyPad6            = 173,
    FDHIDKeyPad7            = 174,
    FDHIDKeyPad8            = 175,
    FDHIDKeyPad9            = 176,
    FDHIDKeyPause           = 255
};

//----------------------------------------------------------------------------------------------------------------------------

typedef NS_ENUM(int, FDHIDMouseAxis)
{
    FDHIDMouseAxisX,
    FDHIDMouseAxisY,
    FDHIDMouseAxisWheel
};

//----------------------------------------------------------------------------------------------------------------------------

typedef NS_ENUM(int, FDHIDGamePadAxis)
{
    FDHIDGamePadAxisLeftX,
    FDHIDGamePadAxisLeftY,
    FDHIDGamePadAxisLeftZ,
    FDHIDGamePadAxisRightX,
    FDHIDGamePadAxisRightY,
    FDHIDGamePadAxisRightZ
};

//----------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    __unsafe_unretained FDHIDDevice*__nullable        mDevice;
    FDHIDEventType      mType;
    unsigned int        mButton;
    
    union
    {
        float           mFloatVal;
        signed int      mIntVal;
        BOOL            mBoolVal;
    };
    
    unsigned int        mPadding;
} FDHIDEvent;

//----------------------------------------------------------------------------------------------------------------------------

@interface FDHIDManager : NSObject

#if __has_feature(objc_class_property)
@property (class, readonly, retain) FDHIDManager *sharedHIDManager;
#else
+ (FDHIDManager*) sharedHIDManager;
#endif
+ (void) checkForIncompatibleDevices;

- (void) setDeviceFilter: (nullable NSArray<NSString*>*) deviceTypes;
@property (readonly, copy) NSArray<FDHIDDevice *> *devices;
- (const FDHIDEvent*) nextEvent;

@end

//----------------------------------------------------------------------------------------------------------------------------

/*
 #if __has_feature(attribute_availability_with_replacement)
 #define __API_R(rep,x) __attribute__((availability(__API_DEPRECATED_PLATFORM_##x,replacement=rep)))
*/
#if __has_feature(attribute_availability_with_replacement)
#define FD_OLD_ENUM(replace) API_DEPRECATED_WITH_REPLACEMENT( replace , macos(10.1, 10.9))
#else
#define FD_OLD_ENUM(replace) DEPRECATED_ATTRIBUTE
#endif

#define FD_HID_OLD_ENUM(typ, val) static const typ e##val FD_OLD_ENUM( #val ) = val

FD_HID_OLD_ENUM(FDHIDEventType, FDHIDEventTypeGamePadAxis);
FD_HID_OLD_ENUM(FDHIDEventType, FDHIDEventTypeGamePadButton);
FD_HID_OLD_ENUM(FDHIDEventType, FDHIDEventTypeKeyboard);
FD_HID_OLD_ENUM(FDHIDEventType, FDHIDEventTypeMouseAxis);
FD_HID_OLD_ENUM(FDHIDEventType, FDHIDEventTypeMouseButton);
FD_HID_OLD_ENUM(FDHIDMouseAxis, FDHIDMouseAxisX);
FD_HID_OLD_ENUM(FDHIDMouseAxis, FDHIDMouseAxisY);
FD_HID_OLD_ENUM(FDHIDMouseAxis, FDHIDMouseAxisWheel);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisLeftX);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisLeftY);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisLeftZ);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisRightX);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisRightY);
FD_HID_OLD_ENUM(FDHIDGamePadAxis, FDHIDGamePadAxisRightZ);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyTab);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyEnter);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyEscape);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeySpace);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyBackspace);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyUpArrow);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyDownArrow);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyLeftArrow);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyRightArrow);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyAlternate);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyOption);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyControl);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyShift);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF1);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF2);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF3);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF4);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF5);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF6);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF7);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF8);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF9);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF10);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF11);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF12);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyInsert);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyDelete);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPageDown);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPageUp);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyHome);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyEnd);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyCapsLock);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyCommand);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyNumLock);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF13);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF14);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF15);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyF16);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadEqual);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadSlash);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadAsterisk);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadMinus);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadPlus);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadEnter);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPadPeriod);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad0);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad1);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad2);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad3);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad4);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad5);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad6);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad7);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad8);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPad9);
FD_HID_OLD_ENUM(FDHIDKey, FDHIDKeyPause);

#undef FD_HID_OLD_ENUM

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_END
