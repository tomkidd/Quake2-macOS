//----------------------------------------------------------------------------------------------------------------------------
//
// "FDAudioFile.m" - Music playback.
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDAudioFile.h"
#import "FDAudioInternal.h"
#import "FDDebug.h"

#import <Cocoa/Cocoa.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>

//----------------------------------------------------------------------------------------------------------------------------

typedef NS_ENUM(NSInteger, FDAudioFileStatus) {
    eFDAudioFileStatusIdle,
    eFDAudioFileStatusPlaying,
    eFDAudioFileStatusPaused,
    eFDAudioFileStatusFinished,
    eFDAudioFileStatusSuspended
};

//----------------------------------------------------------------------------------------------------------------------------

static void FDAudioFile_CompletionProc (void* pUserData, ScheduledAudioFileRegion* pFileRegion, OSStatus result);

//----------------------------------------------------------------------------------------------------------------------------

@interface FDAudioFile()


- (OSStatus) startAtFrame: (SInt64) startFrame loop: (BOOL) loop;
- (void) applicationWillHide: (NSNotification*) notification;
- (void) applicationWillUnhide: (NSNotification*) notification;

@end

//----------------------------------------------------------------------------------------------------------------------------

@implementation FDAudioFile
{
@private
    FDAudioMixer*       mMixer;
    AudioUnitElement    mBusNumber;
    AUNode              mAudioNode;
    AudioUnit           mAudioUnit;
    AudioFileID         mFileId;
    
    SInt64              mPosition;
    FDAudioFileStatus   mStatus;
    
    BOOL                mIsLooping;
}
@synthesize loops = mIsLooping;

- (instancetype) init
{
    self = [self initWithMixer:nil];
    
    if (self != nil)
    {
        [self doesNotRecognizeSelector: _cmd];
    }
    
    return nil;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) initWithMixer: (FDAudioMixer*) mixer
{
    self = [super init];
    
    if (self)
    {
        AUGraph     audioGraph      = 0;
        Boolean     graphWasRunning = false;
        OSStatus    err             = noErr - 1;
        
        if (mixer != nil)
        {
            mMixer      = mixer;
            mBusNumber  = [mixer allocateBus];
            audioGraph  = mixer.audioGraph;
            
            err = AUGraphIsRunning (audioGraph, &graphWasRunning);
            
        }
        
        if ((err == noErr) && graphWasRunning)
        {
            err = AUGraphStop (audioGraph);
        }
        
        if (err == noErr)
        {
            AudioComponentDescription	compDesc    = { 0 };
            
            compDesc.componentType          = kAudioUnitType_Generator;
            compDesc.componentSubType       = kAudioUnitSubType_AudioFilePlayer;
            compDesc.componentManufacturer  = kAudioUnitManufacturer_Apple;
            
            err = AUGraphAddNode (audioGraph, &compDesc, &mAudioNode);
        }
        
        if (err == noErr)
        {
            err = AUGraphNodeInfo (audioGraph, mAudioNode, 0, &mAudioUnit);
        }
        
        if (err == noErr)
        {
            AUNode  mixerNode = mixer.mixerNode;
            
            err = AUGraphConnectNodeInput (audioGraph, mAudioNode, 0, mixerNode, mBusNumber);
        }
        
        if ((err == noErr) && graphWasRunning)
        {
            err = AUGraphStart (audioGraph);
        }
        
        if (err == noErr)
        {
            [mixer addObserver: self];
        }
        
        if (err == noErr)
        {
            mStatus = eFDAudioFileStatusIdle;
        }
        
        if (err != noErr)
        {
            return nil;
        }
    }
    
    return self;
}

//---------------------------------------------------------------------------------------------------------------------------

- (void) dealloc
{
    if (mMixer != nil)
    {
        AUGraph     audioGraph      = mMixer.audioGraph;
        AUNode      mixerNode       = mMixer.mixerNode;
        Boolean     graphWasRunning = false;
        
        [self stop];

        AUGraphIsRunning (audioGraph, &graphWasRunning);
        
        if (graphWasRunning)
        {
            AUGraphStop (audioGraph);
        }
        
        AUGraphDisconnectNodeInput (audioGraph, mixerNode, mBusNumber);
        AUGraphRemoveNode (audioGraph, mAudioNode);
        
        if (graphWasRunning)
        {
            AUGraphStart (audioGraph);
        }
        
        [mMixer removeObserver: self];
        [mMixer deallocateBus: mBusNumber];
        mMixer = nil;
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) setVolume: (float) volume
{
    [mMixer setVolume: volume forBus: mBusNumber];
}

//----------------------------------------------------------------------------------------------------------------------------

- (float) volume
{
    return [mMixer volumeForBus: mBusNumber];
}
 
//----------------------------------------------------------------------------------------------------------------------------

- (OSStatus) startAtFrame: (SInt64) startFrame loop: (BOOL) loop
{
    AudioStreamBasicDescription fileFormat  = { 0 };
    UInt64                      numPackets  = 0;
    OSStatus                    err         = noErr;
    
    if (err == noErr)
    {
        UInt32 propSize = sizeof (numPackets);
        
        err = AudioFileGetProperty (mFileId, kAudioFilePropertyAudioDataPacketCount, &propSize, &numPackets);
    }
    
    if (err == noErr)
    {
        UInt32 propSize = sizeof (AudioStreamBasicDescription);
        
        err = AudioFileGetProperty (mFileId, kAudioFilePropertyDataFormat, &propSize, &fileFormat);
    }
    
    if (err == noErr)
    {
        ScheduledAudioFileRegion region = { 0 };
        
        region.mTimeStamp.mFlags        = kAudioTimeStampSampleTimeValid;
        region.mTimeStamp.mSampleTime   = 0;
        region.mCompletionProc          = &FDAudioFile_CompletionProc;
        region.mCompletionProcUserData  = &mStatus;
        region.mAudioFile               = mFileId;
        region.mLoopCount               = (loop == YES) ? -1 : 0;
        region.mStartFrame              = startFrame;
        region.mFramesToPlay            = (UInt32)(numPackets * fileFormat.mFramesPerPacket);
        
        mIsLooping                      = loop;
        
        err = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_ScheduledFileRegion, kAudioUnitScope_Global, 0,
                                    &region, sizeof (region));
    }
    
    if (err == noErr)
    {
        UInt32 defaultVal = 0;
        
        err = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_ScheduledFilePrime,  kAudioUnitScope_Global, 0,
                                    &defaultVal, sizeof (defaultVal));
    }
    
    if (err == noErr)
    {
        AudioTimeStamp startTime = { 0 };
        
        startTime.mFlags        = kAudioTimeStampSampleTimeValid;
        startTime.mSampleTime   = -1;
        
        err = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_ScheduleStartTimeStamp, kAudioUnitScope_Global, 0,
                                    &startTime, sizeof (startTime));
    }
    
    if (err == noErr)
    {
       mStatus = eFDAudioFileStatusPlaying;
    }
    
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) play
{
    OSStatus err = 0;
    switch (mStatus) {
        case eFDAudioFileStatusPaused:
            err = [self startAtFrame: mPosition loop: mIsLooping];
            break;
            
        case eFDAudioFileStatusPlaying:
            err = noErr;
            break;
            
        case eFDAudioFileStatusFinished:
            mPosition = 0;
            err = [self startAtFrame: mPosition loop: mIsLooping];
            break;
            
        case eFDAudioFileStatusIdle:
        case eFDAudioFileStatusSuspended:
        default:
            err = -1; // generic failure value
            break;
    }

    return err == noErr;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) startFile: (NSURL*) url loop: (BOOL) loop
{
    [self stop];
    
    OSStatus    err     = AudioFileOpenURL ((__bridge CFURLRef)url, kAudioFileReadPermission, 0, &mFileId);
    
    if (err == noErr)
    {
        err = AudioUnitSetProperty (mAudioUnit, kAudioUnitProperty_ScheduledFileIDs, kAudioUnitScope_Global, 0,
                                    &mFileId, sizeof (mFileId));
    }
    
    if (err == noErr)
    {
        _file = url;
        err = [self startAtFrame: 0 loop: loop];
    }
    
    return err == noErr;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) stop
{
    OSStatus err = AudioUnitReset (mAudioUnit, kAudioUnitScope_Global, 0);
    
    if (err == noErr)
    {
        mStatus = eFDAudioFileStatusIdle;
    }
    else
    {
        FDLog (@"FDAudioFile: Failed to stop playback!\n");
    }
    
    if (mFileId != NULL)
    {
        err = AudioFileClose (mFileId);
        
        if ( err != noErr )
        {
            FDLog (@"FDAudioFile: Failed to close file!\n");
        }
        
        mFileId = NULL;
        _file = nil;
    }
    
    return err == noErr;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) restart
{
    if (mStatus == eFDAudioFileStatusIdle)
    {
        return NO;
    }
    
    FDAudioFileStatus oldStatus = mStatus;
    OSStatus err = 0;
    if (mStatus == eFDAudioFileStatusPlaying)
    {
        [self pause];
    }
    else if (oldStatus == eFDAudioFileStatusFinished)
    {
        err = AudioUnitReset (mAudioUnit, kAudioUnitScope_Global, 0);
        if (err == noErr)
        {
            mStatus = eFDAudioFileStatusPaused;
        }
    }
    
    if (err == noErr)
    {
        mPosition = 0;
    }
    
    if (oldStatus == eFDAudioFileStatusPlaying)
    {
        [self resume];
    }
    
    return err == noErr;
}

//---------------------------------------------------------------------------------------------------------------------------

- (void) pause
{
    if (mStatus == eFDAudioFileStatusPlaying)
    {
        AudioTimeStamp  time    = { 0 };
        UInt32          size    = sizeof (time);
        OSStatus        err     = noErr;
        
        err = AudioUnitGetProperty (mAudioUnit, kAudioUnitProperty_CurrentPlayTime, kAudioUnitScope_Global, 0, &time, &size);
        
        if (err == noErr)
        {
            mPosition = time.mSampleTime;
        }
        
        err = AudioUnitReset (mAudioUnit, kAudioUnitScope_Global, 0);
        
        if (err == noErr)
        {
            mStatus = eFDAudioFileStatusPaused;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------

- (void) resume
{
    if (mStatus == eFDAudioFileStatusPaused)
    {
        [self startAtFrame: mPosition loop: mIsLooping];
    }
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) isPlaying
{
    return mStatus == eFDAudioFileStatusPlaying;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) isFinished
{
    return mStatus == eFDAudioFileStatusFinished;
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) applicationWillHide: (NSNotification*) notification
{
    FD_UNUSED (notification);
    
    if (mStatus == eFDAudioFileStatusPlaying)
    {
        [self pause];
        
        if (mStatus == eFDAudioFileStatusPaused)
        {
            mStatus = eFDAudioFileStatusSuspended;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) applicationWillUnhide: (NSNotification*) notification
{
    FD_UNUSED (notification);
    
    if (mStatus == eFDAudioFileStatusSuspended)
    {
        mStatus = eFDAudioFileStatusPaused;
        
        [self resume];
    }
}

@end

//----------------------------------------------------------------------------------------------------------------------------

void FDAudioFile_CompletionProc (void* pUserData, ScheduledAudioFileRegion* pFileRegion, OSStatus result)
{
    FD_UNUSED (pFileRegion, result);
    
    FDAudioFileStatus*  pStatus = (FDAudioFileStatus*) pUserData;
    
    if (pStatus != nil)
    {
        *pStatus = eFDAudioFileStatusFinished;
    }
}

//----------------------------------------------------------------------------------------------------------------------------
