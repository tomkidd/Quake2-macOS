//----------------------------------------------------------------------------------------------------------------------------
//
// "FDDebug.h"
//
// Written by:	Axel 'awe' Wefers			[mailto:awe@fruitz-of-dojo.de].
//				©2001-2012 Fruitz Of Dojo 	[http://www.fruitz-of-dojo.de].
//
//----------------------------------------------------------------------------------------------------------------------------

#import "FDDefines.h"
#import <Foundation/Foundation.h>

//----------------------------------------------------------------------------------------------------------------------------

typedef BOOL (*FDDebugAssertHandler) (const char* __null_unspecified pFile, unsigned int line, const char* __null_unspecified pFormat, ...) __printflike(3, 4);
typedef void (*FDDebugErrorHandler) (const char* __null_unspecified pFormat, ...) __printflike(1, 2);
typedef void (*FDDebugExceptionHandler) (const char* __null_unspecified pFormat, ...) __printflike(1, 2);
typedef void (*FDDebugLogHandler) (const char* __null_unspecified pFormat, ...) __printflike(1, 2);

//----------------------------------------------------------------------------------------------------------------------------

NS_ASSUME_NONNULL_BEGIN

@interface FDDebug : NSObject

#if __has_feature(objc_class_property)
@property (class, readonly, retain) FDDebug *sharedDebug;
@property (class, readonly, getter=isDebuggerAttached) BOOL debuggerAttached;
#else
+ (FDDebug*) sharedDebug;
+ (BOOL) isDebuggerAttached;
#endif

- (instancetype) init;
- (instancetype) initWithName: (NSString*) name NS_DESIGNATED_INITIALIZER;

@property (nonatomic, copy) NSString *name;

@property FDDebugAssertHandler assertHandler;
@property FDDebugErrorHandler errorHandler;
@property FDDebugExceptionHandler exceptionHandler;
@property FDDebugLogHandler logHandler;

- (void) logWithFormat: (NSString*) format, ... NS_FORMAT_FUNCTION(1,2);
- (void) logWithFormat: (NSString*) format arguments: (va_list) argList;

- (void) errorWithFormat: (NSString*) format, ... NS_FORMAT_FUNCTION(1,2);
- (void) errorWithFormat: (NSString*) format arguments: (va_list) argList;

- (void) exception: (NSException*) exception;
- (void) exceptionWithFormat: (NSString*) format, ... NS_FORMAT_FUNCTION(1,2);
- (void) exceptionWithFormat: (NSString*) format arguments: (va_list) argList;

- (BOOL) assert: (NSString*) file line: (NSUInteger) line format: (NSString*) format, ... NS_FORMAT_FUNCTION(3,4);
- (BOOL) assert: (NSString*) file line: (NSUInteger) line format: (NSString*) format arguments: (va_list) argList;

@end

//----------------------------------------------------------------------------------------------------------------------------

FD_EXTERN void    FDLog (NSString* format, ...) NS_FORMAT_FUNCTION(1,2);
FD_EXTERN void    FDLogv (NSString* format, va_list list);
FD_EXTERN void    FDError (NSString* format, ...) NS_FORMAT_FUNCTION(1,2);
FD_EXTERN void    FDErrorv (NSString* format, va_list list);

NS_ASSUME_NONNULL_END

//----------------------------------------------------------------------------------------------------------------------------

#if defined (DEBUG)

#define FD_ASSERT(expr)         if (!(expr))                                                                            \
                                {                                                                                       \
                                    if (![[FDDebug sharedDebug] assert: @""__FILE__ line: __LINE__ format: @""#expr])   \
                                    {                                                                                   \
                                        FD_TRAP ();                                                                     \
                                    }                                                                                   \
                                }

#else

#define FD_ASSERT(expr)         do {} while (0)

#endif // DEBUG

//----------------------------------------------------------------------------------------------------------------------------

#define	FD_DURING               NS_DURING
#define FD_HANDLER              NS_HANDLER                                                                              \
                                {                                                                                       \
                                    [[FDDebug sharedDebug] exception: localException];                                  \
                                }                                                                                       \
                                NS_ENDHANDLER

//----------------------------------------------------------------------------------------------------------------------------
