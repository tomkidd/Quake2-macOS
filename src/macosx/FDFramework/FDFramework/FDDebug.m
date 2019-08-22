//----------------------------------------------------------------------------------------------------------------------------
//
// "FDDebug.m"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDebug.h"
#import "FDDefines.h"

#import <Cocoa/Cocoa.h>
#include <stdarg.h>
#include <sys/sysctl.h>

//----------------------------------------------------------------------------------------------------------------------------

static FDDebug*         sFDDebugInstance    = nil;
static NSString*        sFDDebugDefaultName = @"";

//----------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------

@implementation FDDebug
{
@private
    NSString*               mName;
    NSString*               mLogPrefix;
    FDDebugAssertHandler    mpAssertHandler;
    FDDebugErrorHandler     mpErrorHandler;
    FDDebugExceptionHandler mpExceptionHandler;
    FDDebugLogHandler       mpLogHandler;
}
@synthesize name = mName;
@synthesize assertHandler = mpAssertHandler;
@synthesize errorHandler = mpErrorHandler;
@synthesize exceptionHandler = mpExceptionHandler;
@synthesize logHandler = mpLogHandler;

//----------------------------------------------------------------------------------------------------------------------------

+ (BOOL) isDebuggerAttached
{
    BOOL                isAttached  = NO;
    int                 mib[4]      = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
    struct kinfo_proc   info        = { 0 };
    size_t              size        = sizeof (info);
    
    if (sysctl (mib, FD_SIZE_OF_ARRAY (mib), &info, &size, NULL, 0) == 0)
    {
        isAttached = ((info.kp_proc.p_flag & P_TRACED) != 0);
    }
    
    return isAttached;
}

- (void) dealloc
{
    [mName release];
    [mLogPrefix release];
    
    if (self == sFDDebugInstance)
    {
        sFDDebugInstance = nil;
    }
    
    [super dealloc];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) setName: (NSString*) name
{
    [mName release];
    [mLogPrefix release];
    
    mName = [[NSString alloc] initWithString: name];
    
    if ([name length])
    {
        mLogPrefix = [[NSString alloc] initWithFormat: @"[%@] ", name];
    }
    else
    {
        mLogPrefix = [[NSString alloc] init];
    }
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) logWithFormat: (NSString*) format, ...
{
    va_list argList;
    
    va_start (argList, format);
    
    [self logWithFormat: format arguments: argList];
    
    va_end (argList);
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) logWithFormat: (NSString*) format arguments: (va_list) argList
{
    NSString* msg = [[NSString alloc] initWithFormat: format arguments: argList];
     
    if (mpLogHandler)
    {
        mpLogHandler ([msg cStringUsingEncoding: NSUTF8StringEncoding]);
    }
    else
    {
        NSLog (@"%@%@", mLogPrefix, msg);
    }
    
    [msg release];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) errorWithFormat: (NSString*) format, ...
{
    va_list argList;
    
    va_start (argList, format);
    
    [self errorWithFormat: format arguments: argList];
    
    va_end (argList);
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) errorWithFormat: (NSString*) format arguments: (va_list) argList
{
    NSString* msg = [[NSString alloc] initWithFormat: format arguments: argList];

    if (mpErrorHandler)
    {
        mpErrorHandler ([msg cStringUsingEncoding: NSUTF8StringEncoding]);
    }
    else
    {
        NSLog (@"%@An error has occured: %@\n", mLogPrefix, msg);
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = @"An error has occured:";
        alert.informativeText = msg;
        alert.alertStyle = NSCriticalAlertStyle;
        [alert runModal];
        [alert release];
    }
    
    [msg release];
    
    exit (EXIT_FAILURE);
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) exception: (NSException*) exception
{
    NSString*   reason = exception.reason;
    
    if (reason == nil)
    {
        reason = @"Unknown exception!";
    }
    
    [self exceptionWithFormat: @"%@", reason];
}

//----------------------------------------------------------------------------------------------------------------------------

- (void) exceptionWithFormat: (NSString*) format, ...
{
    va_list argList;
    
    va_start (argList, format);
    
    [self errorWithFormat: format arguments: argList];
    
    va_end (argList);
}

//---------------------------------------------------------------------------------------------------------------------------

- (void) exceptionWithFormat: (NSString*) format arguments: (va_list) argList
{
    NSString* msg = [[NSString alloc] initWithFormat: format arguments: argList];
    
    if (mpExceptionHandler)
    {
        mpExceptionHandler ([msg cStringUsingEncoding: NSUTF8StringEncoding]);
    }
    else
    {
        NSLog (@"%@An exception has occured: %@\n", mLogPrefix, msg);
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = @"An exception has occured:";
        alert.informativeText = msg;
        alert.alertStyle = NSCriticalAlertStyle;
        [alert runModal];
        [alert release];
    }
    [msg release];
}

//----------------------------------------------------------------------------------------------------------------------------

- (BOOL) assert: (NSString*) file line: (NSUInteger) line format: (NSString*) format, ...
{
    BOOL resume = NO;
    
    va_list argList;
    
    va_start (argList, format);
    
    resume = [self assert: file line: line format: format arguments: argList];
    
    va_end (argList);
    
    return resume;
}

//---------------------------------------------------------------------------------------------------------------------------

- (BOOL) assert: (NSString*) file line: (NSUInteger) line format: (NSString*) format arguments: (va_list) argList
{
    NSString*   msg     = [[NSString alloc] initWithFormat: format arguments: argList];
    BOOL        resume  = NO;
    
    if ([FDDebug isDebuggerAttached] == NO)
    {
        if (mpAssertHandler)
        {
            const char* pFile   = [file fileSystemRepresentation];
            const char* pMsg    = [msg cStringUsingEncoding: NSUTF8StringEncoding];
            
            resume = mpAssertHandler (pFile, (unsigned int) line, pMsg);
        }
        else
        {
            NSString* dlg = [[NSString alloc] initWithFormat: @"\"%@\" (%lu): %@", file, (unsigned long) line, msg];
            
            NSLog (@"%@%@ (%d): Assertion failed: %@", mLogPrefix, file, (unsigned int) line, msg);
            
            NSAlert *alert = [[NSAlert alloc] init];
            alert.messageText = @"Assertion failed:";
            alert.informativeText = dlg;
            [alert addButtonWithTitle:@"Resume"];
            [alert addButtonWithTitle:@"Crash"];
            alert.alertStyle = NSCriticalAlertStyle;
            
            resume = ([alert runModal] == NSAlertFirstButtonReturn);
            
            [dlg release];
            [alert release];
        }
    }
    
    [msg release];
    
    return resume;
}

//----------------------------------------------------------------------------------------------------------------------------

+ (FDDebug*) sharedDebug
{
    if (!sFDDebugInstance)
    {
        sFDDebugInstance = [[FDDebug alloc] initWithName: sFDDebugDefaultName];
    }
    
    return sFDDebugInstance;
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) init
{
    return [self initWithName:sFDDebugDefaultName];
}

//----------------------------------------------------------------------------------------------------------------------------

- (instancetype) initWithName: (NSString*) name
{
    self = [super init];
    
    if (self !=  nil)
    {
        self.name = name;
    }
    
    return self;
}

@end

//----------------------------------------------------------------------------------------------------------------------------

void    FDLog (NSString* format, ...)
{
    va_list argList;
    
    va_start (argList, format);
    
    FDLogv(format, argList);
    
    va_end (argList); 
}

//----------------------------------------------------------------------------------------------------------------------------

void    FDLogv (NSString* format, va_list argList)
{
    [[FDDebug sharedDebug] logWithFormat: format arguments: argList];
}

//----------------------------------------------------------------------------------------------------------------------------

void    FDError (NSString* format, ...)
{
    va_list argList;
    
    va_start (argList, format);
    
    FDErrorv(format, argList);
    
    va_end (argList);     
}

//----------------------------------------------------------------------------------------------------------------------------

void    FDErrorv (NSString* format, va_list argList)
{
    [[FDDebug sharedDebug] errorWithFormat: format arguments: argList];
}

//---------------------------------------------------------------------------------------------------------------------------
