/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <utility>

#include "Swiften/Network/MacOSXProxyProvider.h"
#include <SystemConfiguration/SystemConfiguration.h>

namespace Swift {

MacOSXProxyProvider::MacOSXProxyProvider()
: ProxyProvider()
{
	CFDictionaryRef proxies = SCDynamicStoreCopyProxies(NULL);
	if(proxies != NULL) {
		socksProxy = getFromDictionary(proxies, kSCPropNetProxiesSOCKSEnable, kSCPropNetProxiesSOCKSProxy, kSCPropNetProxiesSOCKSPort);
		httpProxy = getFromDictionary(proxies, kSCPropNetProxiesHTTPEnable, kSCPropNetProxiesHTTPProxy, kSCPropNetProxiesHTTPPort);
	}
}

HostAddressPort MacOSXProxyProvider::getHTTPConnectProxy() const {
	return httpProxy;
}

HostAddressPort MacOSXProxyProvider::getSOCKS5Proxy() const {
	return socksProxy;
}

HostAddressPort MacOSXProxyProvider::getFromDictionary(CFDictionaryRef dict, CFStringRef enabledKey, CFStringRef hostKey, CFStringRef portKey) {
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
						for(std::vector<char>::iterator iter = buffer.begin(); iter != buffer.end(); iter++) {
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

}
