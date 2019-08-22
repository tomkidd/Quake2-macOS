//----------------------------------------------------------------------------------------------------------------------------
//
// "FDHIDInternal.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDHIDDevice.h"
#import "FDHIDManager.h"
#import "FDHIDActuator.h"

#import <Cocoa/Cocoa.h>
#include <ForceFeedback/ForceFeedback.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hid/IOHIDLib.h>

//----------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint32_t                mUsage;
    uint32_t                mButton;
    void                    (*__nullable mpEventHandler)(__kindof FDHIDDevice*__nonnull, unsigned int, IOHIDValueRef __nonnull, IOHIDElementRef __nullable);
} FDHIDButtonMap;

//----------------------------------------------------------------------------------------------------------------------------

typedef struct 
{
    uint32                      mType;
    uint32                      mNumButtons;
    FDHIDButtonMap*__nullable   mpButtons;
} FDHIDElementMap;

//----------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    SInt32                      mVendorId;
    SInt32                      mProductId;
    FDHIDElementMap*__nullable  mpElements;
    uint32_t                    mNumElements;
    uint32_t                    mPadding;
} FDHIDDeviceDesc;

//----------------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint32_t                    mUsagePage;
    uint32_t                    mUsage;
    FDHIDDeviceDesc*__nullable  mDeviceDesc;
    uint32_t                    mNumDeviceDesc;
    uint32_t                    m_Padding;
} FDHIDUsageToDevice;

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDHIDDevice()
{
    IOHIDDeviceRef          mpIOHIDDevice;
    NSString*               mVendorName;
    NSString*               mProductName;
    FDHIDActuator*          mActuator;
    const FDHIDDeviceDesc*  mpDeviceDesc;
    FDHIDManager*           __weak mDelegate;
}

+ (NSArray<NSDictionary<NSString*,NSNumber*>*>*) matchingDictionaries: (const FDHIDUsageToDevice*) usageMap withCount: (NSUInteger) numUsages;
+ (nullable FDHIDDevice*) deviceWithDevice: (IOHIDDeviceRef) pDevice
                                  usageMap: (const FDHIDUsageToDevice*) pUsageMap
                                     count: (NSUInteger) numUsages;

- (instancetype) initWithDevice: (IOHIDDeviceRef) pDevice deviceDescriptors: (const FDHIDDeviceDesc*) deviceDescriptors;

@property (weak, nullable) FDHIDManager* delegate;
- (void) pushEvent: (const FDHIDEvent*) pEvent;

@property (readonly, assign) IOHIDDeviceRef iohidDeviceRef;
- (void) handleInput: (IOHIDValueRef) pValue;
@property (readonly) FDHIDElementMap* elementMap;
@property (readonly) NSUInteger elementCount;
- (void) flush;

@end

@interface FDHIDDevice (subclassMethods)
+ (nullable FDHIDDevice*) deviceWithDevice: (IOHIDDeviceRef) pDevice;
+ (NSArray<NSDictionary<NSString*,NSNumber*>*>*) matchingDictionaries;
@end

//----------------------------------------------------------------------------------------------------------------------------

@interface FDHIDActuator()
{
    io_service_t            mIoService;
    FFDeviceObjectReference mpDevice;
    FFEffectObjectReference mpEffect;
    FFEFFECT                mEffectParams;
    FFENVELOPE              mEffectEnvelope;
    FFPERIODIC              mEffectPeriodic;
    DWORD                   mEffectAxes[32];
    LONG                    mEffectDirection[2];

}

- (nullable instancetype) initWithDevice: (FDHIDDevice*) device;

@end

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------
