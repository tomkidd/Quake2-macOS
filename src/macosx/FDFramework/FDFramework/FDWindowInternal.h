//
//  FDWindowInternal.h
//  FruitzOfDojo
//
//  Created by C.W. Betts on 5/18/16.
//  Copyright © 2016 C.W. Betts. All rights reserved.
//

#ifndef FDWindowInternal_h
#define FDWindowInternal_h

#import "FDWindow.h"

//----------------------------------------------------------------------------------------------------------------------------

@interface FDView()

- (void) initGrowBoxTexture;
- (void) drawGrowbox;

@property (readwrite, strong) NSOpenGLContext *openGLContext;

- (void) setResizeHandler: (FDResizeHandler) pResizeHandler forContext: (void*) pContext;
- (void) onResizeView: (NSNotification*) notification;

- (NSBitmapImageRep*) bitmapRepresentation;

@end


#endif /* FDWindowInternal_h */
