// Implements the interface to the pty session.

#import <Foundation/Foundation.h>

extern NSString *kCoprocessStatusChangeNotification;

@class Coprocess;
@class PTYTab;

@protocol PTYTaskDelegate <NSObject>
// Runs in a background thread. Should do as much work as possible in this
// thread before kicking off a possibly async task in the main thread.
- (void)threadedReadTask:(char *)buffer length:(int)length;
- (void)brokenPipe;
- (void)taskWasDeregistered;
@end

@interface PTYTask : NSObject

@property(atomic, readonly) BOOL hasMuteCoprocess;
@property(atomic, assign) id<PTYTaskDelegate> delegate;

- (id)init;
- (void)dealloc;
- (BOOL)hasBrokenPipe;
- (NSString *)command;
- (void)launchWithPath:(NSString*)progpath
             arguments:(NSArray*)args
           environment:(NSDictionary*)env
                 width:(int)width
                height:(int)height
                isUTF8:(BOOL)isUTF8;

- (NSString*)currentJob:(BOOL)forceRefresh;

- (void)writeTask:(NSData*)data;

- (void)sendSignal:(int)signo;
- (void)setWidth:(int)width height:(int)height;
- (void)stop;

- (int)fd;
- (pid_t)pid;
- (int)status;
- (NSString*)tty;
- (NSString*)path;
- (NSString*)getWorkingDirectory;
- (NSString*)description;

- (BOOL)loggingStartWithPath:(NSString*)path;
- (void)loggingStop;
- (BOOL)logging;
- (BOOL)hasOutput;

- (BOOL)wantsRead;
- (BOOL)wantsWrite;
- (void)brokenPipe;
- (void)processRead;
- (void)processWrite;

- (void)setCoprocess:(Coprocess *)coprocess;
- (Coprocess *)coprocess;
- (BOOL)writeBufferHasRoom;
- (BOOL)hasCoprocess;
- (void)stopCoprocess;

- (void)logData:(const char *)buffer length:(int)length;

@end

