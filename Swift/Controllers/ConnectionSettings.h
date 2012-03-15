/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

struct ConnectionSettings {
		enum Method {
			Automatic,
			Manual,
			BOSH
		};
		enum ProxyType {
			None,
			System,
			SOCKS5,
			HTTPConnect
		};

		Method method;
		struct {
				bool useManualServer;
				std::string manualServerHostname;
				int manualServerPort;
				ProxyType proxyType;
				bool useManualProxy;
				std::string manualProxyHostname;
				int manualProxyPort;
		} manualSettings;
		struct {
				std::string boshURI;
				bool useManualProxy;
				std::string manualProxyHostname;
				int manualProxyPort;
		} boshSettings;
};
