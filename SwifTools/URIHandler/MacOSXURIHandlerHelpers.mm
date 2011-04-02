/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/URIHandler/MacOSXURIHandlerHelpers.h>

#include <Cocoa/Cocoa.h>

namespace Swift {
	void registerAppAsDefaultXMPPURIHandler() {
		NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
		LSSetDefaultHandlerForURLScheme((CFStringRef)@"xmpp", (CFStringRef)bundleID);
	}
}
