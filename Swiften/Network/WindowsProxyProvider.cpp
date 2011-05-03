/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Network/WindowsProxyProvider.h>
#include <Swiften/Base/ByteArray.h>

#include <windows.h>

namespace Swift {

WindowsProxyProvider::WindowsProxyProvider()
: ProxyProvider()
{
	HKEY hKey = (HKEY)INVALID_HANDLE_VALUE;
	long result;
		
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hKey);
	if (result == ERROR_SUCCESS && hKey != INVALID_HANDLE_VALUE && proxyEnabled(hKey)) {
		DWORD dataType = REG_SZ;
		DWORD dataSize = 0;
		ByteArray dataBuffer;

		result = RegQueryValueEx(hKey, "ProxyServer", NULL, &dataType, NULL, &dataSize);
		if(result != ERROR_SUCCESS) {
			return;
		}
		dataBuffer.resize(dataSize);
		result = RegQueryValueEx(hKey, "ProxyServer", NULL, &dataType, reinterpret_cast<BYTE*>(vecptr(dataBuffer)), &dataSize);
		if(result == ERROR_SUCCESS) {
			std::vector<std::string> proxies = String::split(byteArrayToString(dataBuffer), ';');
			std::pair<std::string, std::string> protocolAndProxy;
			foreach(std::string proxy, proxies) {
				if(proxy.find('=') != std::string::npos) {
					protocolAndProxy = String::getSplittedAtFirst(proxy, '=');
					SWIFT_LOG(debug) << "Found proxy: " << protocolAndProxy.first << " => " << protocolAndProxy.second << std::endl;
					if(protocolAndProxy.first.compare("socks") == 0) {
						socksProxy = getAsHostAddressPort(protocolAndProxy.second);
					}
					else if (protocolAndProxy.first.compare("http") == 0) {
						httpProxy = getAsHostAddressPort(protocolAndProxy.second);
					}
				}
			}
		}
	}
}

HostAddressPort WindowsProxyProvider::getHTTPConnectProxy() const {
	return httpProxy;
}

HostAddressPort WindowsProxyProvider::getSOCKS5Proxy() const {
	return socksProxy;
}

HostAddressPort WindowsProxyProvider::getAsHostAddressPort(std::string proxy) {
	HostAddressPort ret(HostAddress(), 0);

	try {
		std::pair<std::string, std::string> tmp;
		int port = 0;
		tmp = String::getSplittedAtFirst(proxy, ':');
		// .c_str() is needed as tmp.second can include a \0 char which will end in an exception of the lexical cast.
		// with .c_str() the \0 will not be part of the string which is to be casted
		port = boost::lexical_cast<int> (tmp.second.c_str());
		ret = HostAddressPort(HostAddress(tmp.first), port);
	}
	catch(...) {
			std::cerr << "Exception occured while parsing windows proxy \"getHostAddressPort\"." << std::endl;
	}

	return ret;
}


bool WindowsProxyProvider::proxyEnabled(HKEY hKey) const {
	bool ret = false;
	long result;
	DWORD dataType = REG_DWORD;
	DWORD dataSize = 0;
	DWORD data = 0;
	ByteArray dataBuffer;

	if(hKey == INVALID_HANDLE_VALUE)
		return ret;

	result = RegQueryValueEx(hKey, "ProxyEnable", NULL, &dataType, NULL, &dataSize);
	if(result != ERROR_SUCCESS)
		return ret;

	dataBuffer.resize(dataSize);
	result = RegQueryValueEx(hKey, "ProxyEnable", NULL, &dataType, reinterpret_cast<BYTE*>(vecptr(dataBuffer)), &dataSize);
	if(result != ERROR_SUCCESS)
		return ret;

	for(size_t t = 0; t < dataBuffer.size(); t++) {
		data += static_cast<int> (dataBuffer[t]) * pow(256, static_cast<double>(t));
	}
	return (data == 1);
}

}
