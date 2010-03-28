#include "Swiften/Application/MacOSX/MacOSXApplicationMessageDisplay.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

#include "Swiften/Base/String.h"

namespace Swift {

MacOSXApplicationMessageDisplay::MacOSXApplicationMessageDisplay() {
}

void MacOSXApplicationMessageDisplay::setMessage(const String& label) {
	NSString *labelString = [[NSString alloc] initWithUTF8String: label.getUTF8Data()];
	[[NSApp dockTile] setBadgeLabel: labelString];
	[labelString release];
	[NSApp requestUserAttention: NSInformationalRequest];
}

}
