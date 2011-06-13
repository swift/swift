/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Base/Platform.h>
#include <Swiften/Network/MacOSXProxyProvider.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <utility>

#ifndef SWIFTEN_PLATFORM_IPHONE
#include <SystemConfiguration/SystemConfiguration.h>
#endif

using namespace Swift;

#ifndef SWIFTEN_PLATFORM_IPHONE
static HostAddressPort getFromDictionary(CFDictionaryRef dict, CFStringRef enabledKey, CFStringRef hostKey, CFStringRef portKey) {
	CFNumberRef numberValue = NULL;
	HostAddressPort ret = HostAddressPort(HostAddress(), 0);

	if(CFDictionaryGetValueIfPresent(dict, reinterpret_cast<const void*> (enabledKey), reinterpret_cast<const void**> (&numberValue)) == true) {
		const int i = 0;
		CFNumberRef zero = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &i);
		CFComparisonResult result = CFNumberCompare(numberValue, zero, NULL);
		CFRelease(numberValue);

		if(result != kCFCompareEqualTo) {
			int port = 0;
			std::string host = "";

			try {
				CFNumberRef numberValue = reinterpret_cast<CFNumberRef> (CFDictionaryGetValue(dict, portKey));
				if(numberValue != NULL) {
					CFNumberGetValue(numberValue, kCFNumberIntType, &port);
					CFRelease(numberValue);
				}

				CFStringRef stringValue = reinterpret_cast<CFStringRef> (CFDictionaryGetValue(dict, hostKey));
				if(stringValue != NULL) {
					std::vector<char> buffer;
 					// length must be +1 for the ending zero; and the Docu of CFStringGetCString tells it like
					// if the string is toby the length must be at least 5.
					CFIndex length = CFStringGetLength(stringValue) + 1;
					buffer.resize(length);
					if(CFStringGetCString(stringValue, &buffer[0], length, kCFStringEncodingMacRoman)) {
						for(std::vector<char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
							host += *iter;
						}
					}
					CFRelease(stringValue);
				}
			}
			catch(...) {
				std::cerr << "Exception caught ... " << std::endl;
			}
			
			if(host != "" && port != 0) {
				ret = HostAddressPort(HostAddress(host), port);
			}
		}
	}
	return ret;
}
#endif
namespace Swift {

MacOSXProxyProvider::MacOSXProxyProvider() {
}

HostAddressPort MacOSXProxyProvider::getHTTPConnectProxy() const {
	HostAddressPort result;
#ifndef SWIFTEN_PLATFORM_IPHONE
	CFDictionaryRef proxies = SCDynamicStoreCopyProxies(NULL);
	if(proxies != NULL) {
		result = getFromDictionary(proxies, kSCPropNetProxiesHTTPEnable, kSCPropNetProxiesHTTPProxy, kSCPropNetProxiesHTTPPort);
	}
#endif
	return result;
}

HostAddressPort MacOSXProxyProvider::getSOCKS5Proxy() const {
	HostAddressPort result;
#ifndef SWIFTEN_PLATFORM_IPHONE
	CFDictionaryRef proxies = SCDynamicStoreCopyProxies(NULL);
	if(proxies != NULL) {
		result = getFromDictionary(proxies, kSCPropNetProxiesSOCKSEnable, kSCPropNetProxiesSOCKSProxy, kSCPropNetProxiesSOCKSPort);
	}
#endif
	return result;
}

}
