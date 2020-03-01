#import "Window.h"

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

@interface UtilsApplication : NSApplication
{
	NSArray* nibObjects;
}

@end

@implementation UtilsApplication

- (void)sendEvent:(NSEvent*)event
{
	[super sendEvent:event];
}

- (void)doNothing:(id)object
{
}

@end

@interface UtilsApplicationDelegate : NSObject
@end

@implementation UtilsApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	return NSTerminateCancel;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
	[NSApp stop:nil];

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
										location:NSMakePoint(0, 0)
								   modifierFlags:0
									   timestamp:0
									windowNumber:0
										 context:nil
										 subtype:0
										   data1:0
										   data2:0];
	[NSApp postEvent:event atStart:YES];
	[pool drain];
}

@end

namespace Utils {
	
struct Cocoa_Impl
{
	static void initialize()
	{
		if (NSApp)
			return;
		[UtilsApplication sharedApplication];

		[NSThread detachNewThreadSelector:@selector(doNothing:) toTarget:NSApp withObject:nil];

		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

		NSMenu* menubar = [NSMenu new];
		[NSApp setMainMenu:menubar];

		id delegate = [[UtilsApplicationDelegate alloc] init];

		[NSApp setDelegate:delegate];

		[NSApp run];
	}
};

struct WindowMac_Impl
{
	NSWindow* window = nullptr;
	NSAutoreleasePool* pool = nullptr;

	WindowMac_Impl(const char* title, Vec2I windowSize)
	{
		int width = windowSize.X;
		int height = windowSize.Y;

		NSRect frame = NSMakeRect(0, 0, width, height);
		window = [[NSWindow alloc] initWithContentRect:frame
											 styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
											   backing:NSBackingStoreBuffered
												 defer:NO];

		window.title = [NSString stringWithCString:title encoding:NSUTF8StringEncoding];
		window.releasedWhenClosed = false;
		[window center];
		[window orderFrontRegardless];

		pool = [[NSAutoreleasePool alloc] init];
	}

	~WindowMac_Impl()
	{
		if (window != nullptr)
		{
			[window release];
			window = nullptr;
		}

		[pool drain];
	}

	void gc()
	{
		[pool drain];
		pool = [[NSAutoreleasePool alloc] init];
	}

	bool newFrame()
	{
		for (;;)
		{
			NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
												untilDate:[NSDate distantPast]
												   inMode:NSDefaultRunLoopMode
												  dequeue:YES];
			if (event == nil)
				break;

			[NSApp sendEvent:event];
		}

		gc();

		if (!window.isVisible)
		{
			return false;
		}

		return true;
	}
};

bool WindowMac::Initialize(const char* title, const Vec2I& windowSize)
{
	Cocoa_Impl::initialize();
	impl = std::make_shared<WindowMac_Impl>(title, windowSize);
    windowSize_ = windowSize;
	return true;
}

bool WindowMac::DoEvent() { return impl->newFrame(); }

void WindowMac::Terminate() { impl.reset(); }

void* WindowMac::GetNSWindowAsVoidPtr() { return impl->window; }

bool WindowMac::OnNewFrame()
{
    return DoEvent();
}
    
void* WindowMac::GetNativePtr(int32_t index)
{
    return GetNSWindowAsVoidPtr();
}
    
Vec2I WindowMac::GetWindowSize() const
{
    return windowSize_;
}
    
} // namespace Utils
