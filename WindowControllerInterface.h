// Defines a protocol shared by PseudoTerminal and FakeWindow.

#import <Cocoa/Cocoa.h>
#import "ProfileModel.h"

@class PSMTabBarControl;
@class PTYSession;
@class PTYTabView;
@class PTYTab;
@class TmuxController;

typedef enum {
    BROADCAST_OFF,
    BROADCAST_TO_ALL_PANES,
    BROADCAST_TO_ALL_TABS,
    BROADCAST_CUSTOM
} BroadcastMode;

// This is a very basic interface, which is sufficient for simulating a window
// controller for instant replay.
@protocol WindowControllerInterface <NSObject>

// Called by VT100Screen when it wants to resize a window for a
// session-initiated resize. It resizes the session, then the window, then all
// sessions to fit the new window size.
- (void)sessionInitiatedResize:(PTYSession*)session width:(int)width height:(int)height;

// Is the window in traditional fullscreen mode?
- (BOOL)fullScreen;

// Returns true if the window is fullscreen in either Lion-style or
// pre-Lion-style fullscreen.
- (BOOL)anyFullScreen;

// Close a session
- (void)closeSession:(PTYSession*)aSession;

// Select the tab to the right of the foreground tab.
- (void)nextTab:(id)sender;

// Select the tab to the left of the foreground tab.
- (void)previousTab:(id)sender;

// Set the text color for a tab control's name.
- (void)setLabelColor:(NSColor *)color forTabViewItem:tabViewItem;

// Set background color for tab chrome.
- (void)setTabColor:(NSColor *)color forTabViewItem:tabViewItem;

// Returns the background color for tab chrome.
- (NSColor*)tabColorForTabViewItem:(NSTabViewItem*)tabViewItem;

// Set blur radius for window.
- (void)enableBlur:(double)radius;

// Disable blur for window.
- (void)disableBlur;

// Is the window title transient?
- (BOOL)tempTitle;

// Force the window size to change to be just large enough to fit this session.
- (void)fitWindowToTab:(PTYTab*)tab;

// Accessor for window's tab view.
- (PTYTabView *)tabView;

// accessor for foreground session.
- (PTYSession *)currentSession;

// Set the window title to the name of the current session.
- (void)setWindowTitle;

// Set the window title to non-transient.
- (void)resetTempTitle;

// Return the foreground tab
- (PTYTab*)currentTab;

// Kill tmux window if applicable, or close a tab and resize/close the window if needed.
- (void)closeTab:(PTYTab*)theTab;

// WindowControllerInterface protocol
- (void)windowSetFrameTopLeftPoint:(NSPoint)point;
- (void)windowPerformMiniaturize:(id)sender;
- (void)windowDeminiaturize:(id)sender;
- (void)windowOrderFront:(id)sender;
- (void)windowOrderBack:(id)sender;
- (BOOL)windowIsMiniaturized;
- (NSRect)windowFrame;
- (NSScreen*)windowScreen;

// Indicates if the scroll bar should be shown.
- (BOOL)scrollbarShouldBeVisible;

// Gives the type of scroller to use.
- (NSScrollerStyle)scrollerStyle;

@end

// The full interface for a window controller, as seen by objects that treat it
// like a delegate.
@protocol iTermWindowController <WindowControllerInterface>

#pragma mark - Basics

// Get term number
- (int)number;

// Underlying window
- (NSWindow *)window;

// For window restoration, take a new snapshot of the current view hierarchy.
- (void)invalidateRestorableState;

// Open a new window with a profile with the specified GUID.
- (void)newWindowWithBookmarkGuid:(NSString*)guid;

// Open a new tab with a profile with the specified GUID.
- (void)newTabWithBookmarkGuid:(NSString*)guid;

// Construct the right-click context menu.
- (void)menuForEvent:(NSEvent *)theEvent menu:(NSMenu *)theMenu;

// Resize the window to a given pixel size. A nearby size will be used if
// possible, but minimum size constraints will be respected.
- (void)setFrameSize:(NSSize)newSize;

// Is transparency being used?
- (BOOL)useTransparency;

// Increment the badge count, or set it to 1 if there is none.
- (void)incrementBadge;

// For scripting.
- (NSScriptObjectSpecifier *)objectSpecifier;

// Indicates if progress indicators can be shown. They're turned off during
// animation of the tabbar.
- (BOOL)disableProgressIndicators;

// Last time the window was resized.
- (NSDate *)lastResizeTime;

// The current mode for broadcasting of input.
- (BroadcastMode)broadcastMode;

// Returns true if the window is in 10.7-style fullscreen.
- (BOOL)lionFullScreen;

// Get the window type
- (int)windowType;

// Returns a new terminal at the given screen coordinate. The
// "wasDraggedFromAnotherWindow_" flag is set on the returned window.
- (NSWindowController<iTermWindowController> *)terminalDraggedFromAnotherWindowAtPoint:(NSPoint)point;

// Session terminated. Remove any weak refs to it.
- (void)sessionDidTerminate:(PTYSession *)session;

#pragma mark - Tabs

// Close a tab and resize/close the window if needed.
- (void)removeTab:(PTYTab *)aTab;

// Move tabs within ordering.
- (void)moveTabLeft:(id)sender;
- (void)moveTabRight:(id)sender;

// If soft is true, don't kill tmux session. Otherwise is just like closeTab.
- (void)closeTab:(PTYTab *)aTab soft:(BOOL)soft;

// Accessor for tab bar.
- (PSMTabBarControl*)tabBarControl;

// Return the number of tabs in this window.
- (int)numberOfTabs;

// Adds a tab to the end.
- (void)appendTab:(PTYTab*)aTab;

// Fit the window to exactly fit a tab of the given size. Returns true if the
// window was resized.
- (BOOL)fitWindowToTabSize:(NSSize)tabSize;

// Return the index of a tab or NSNotFound.
// This method is used, for example, in iTermExpose, where PTYTabs are shown
// side by side, and one needs to determine which index it has, so it can be
// selected when leaving iTerm expose.
- (NSInteger)indexOfTab:(PTYTab*)aTab;

// Insert a tab at a specified location.
- (void)insertTab:(PTYTab*)aTab atIndex:(int)anIndex;

// Add a session to the tab view.
- (void)insertSession:(PTYSession *)aSession atIndex:(int)anIndex;

// Resize window to be just large enough to fit the largest tab without
// changing session sizes.
- (void)fitWindowToTabs;

#pragma mark - Sessions

// Set the session name. If theSessionName is nil then set it to the pathname
// or "Finish" if it's closed.
- (void)setName:(NSString *)theSessionName forSession:(PTYSession*)aSession;

// For each instance of a substring like $$foo$$ in command and name,
// prompt the user for a value and replace $$foo$$ with the user-entered
// value in both command and name.
- (void)getSessionParameters:(NSMutableString *)command
                    withName:(NSMutableString *)name;

// Return the name of the foreground session.
- (NSString *)currentSessionName;

// Show the pref panel for the current session, divorcing it from its profile.
- (void)editSession:(PTYSession*)session;

// Close a session if the user agrees to a modal alert.
- (void)closeSessionWithConfirmation:(PTYSession *)aSession;

// Close a session but don't kill the underlying window pane if it's a tmux session.
- (void)softCloseSession:(PTYSession *)aSession;

// Update sessions' dimming status.
- (void)setDimmingForSessions;

// All sessions in this winodw.
- (NSArray*)sessions;

// Do some cleanup after a session is removed.
- (void)sessionWasRemoved;

#pragma mark - Instant replay

// Begin instant replay on a session.
- (void)replaySession:(PTYSession *)oldSession;

// End instant replay, subbing in a live sesssion for the fake IR session.
- (void)showLiveSession:(PTYSession*)liveSession
              inPlaceOf:(PTYSession*)replaySession;

// Is the window currently in IR mode?
- (BOOL)inInstantReplay;

// Hide the IR bar and end instant replay.
- (void)closeInstantReplay:(id)sender;

// Step forward/back in IR.
- (void)irPrev:(id)sender;
- (void)irNext:(id)sender;

// Toggle the visibility of IR.
- (void)showHideInstantReplay;

#pragma mark - Broadcast

// Indicates if a session participates in input broadcasting.
- (BOOL)broadcastInputToSession:(PTYSession *)session;

// Toggles broadcasting to a single session.
- (void)toggleBroadcastingInputToSession:(PTYSession *)session;

// Call writeTask: for each session's shell with the given data.
- (void)sendInputToAllSessions:(NSData *)data;

#pragma mark - Tmux

// Returns the size of the window (in characters) that is the smallest width of
// any tab and smallest height of any tab.
- (NSSize)tmuxCompatibleSize;

// Increment the count of tmux originated resizes in progress.
- (void)beginTmuxOriginatedResize;

// Decrement the count of tmux originated resizes in progress.
- (void)endTmuxOriginatedResize;

// Fit the window to the tabs after a tmux layout change. A change is trivial
// if views are resized but the view hierarchy is not changed.
- (void)tmuxTabLayoutDidChange:(BOOL)nontrivialChange;

// Returns an array of unique tmux controllers present in this window.
- (NSArray *)uniqueTmuxControllers;

// Opens a new tmux tab. window gives the tmux window id. name gives the new
// window title.
- (void)loadTmuxLayout:(NSMutableDictionary *)parseTree
                window:(int)window
        tmuxController:(TmuxController *)tmuxController
                  name:(NSString *)name;

#pragma mark - Splits

// Create a new split. The new session uses the profile with |guid|.
- (void)splitVertically:(BOOL)isVertical withBookmarkGuid:(NSString*)guid;

// Create a new split with a specified bookmark. |targetSession| is the session
// to split.
- (void)splitVertically:(BOOL)isVertical
           withBookmark:(Profile*)theBookmark
          targetSession:(PTYSession*)targetSession;

// Create a new split with the specified bookmark. The passed-in session is
// inserted either before (left/above) or after (right/below) the target
// session. If performSetup is set, then setupSession:title:withSize: is
// called.
- (void)splitVertically:(BOOL)isVertical
                 before:(BOOL)before
          addingSession:(PTYSession*)newSession
          targetSession:(PTYSession*)targetSession
           performSetup:(BOOL)performSetup;

// Indicates if the current session can be split.
- (BOOL)canSplitPaneVertically:(BOOL)isVertical withBookmark:(Profile*)theBookmark;

// Indicates if this the hotkey window.
- (BOOL)isHotKeyWindow;

@end
