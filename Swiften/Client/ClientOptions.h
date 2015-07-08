/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/URL.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/TLS/TLSOptions.h>


namespace Swift {
	class HTTPTrafficFilter;

	struct SWIFTEN_API ClientOptions {
		enum UseTLS {
			NeverUseTLS,
			UseTLSWhenAvailable,
			RequireTLS
		};

		enum ProxyType {
			NoProxy,
			SystemConfiguredProxy,
			SOCKS5Proxy,
			HTTPConnectProxy
		};

		ClientOptions() : 
				useStreamCompression(true), 
				useTLS(UseTLSWhenAvailable), 
				allowPLAINWithoutTLS(false), 
				useStreamResumption(false), 
				forgetPassword(false), 
				useAcks(true), 
				singleSignOn(false),
				manualHostname(""),
				manualPort(-1),
				proxyType(SystemConfiguredProxy),
				manualProxyHostname(""),
				manualProxyPort(-1),
				boshHTTPConnectProxyAuthID(""), 
				boshHTTPConnectProxyAuthPassword("") {
		}

		/**
		 * Whether ZLib stream compression should be used when available.
		 *
		 * Default: true
		 */
		bool useStreamCompression;

		/**
		 * Sets whether TLS encryption should be used.
		 *
		 * Default: UseTLSWhenAvailable
		 */
		UseTLS useTLS;

		/**
		 * Sets whether plaintext authentication is 
		 * allowed over non-TLS-encrypted connections.
		 *
		 * Default: false
		 */
		bool allowPLAINWithoutTLS;

		/**
		 * Use XEP-196 stream resumption when available.
		 *
		 * Default: false
		 */
		bool useStreamResumption;

		/**
		 * Forget the password once it's used.
		 * This makes the Client useless after the first login attempt.
		 *
		 * FIXME: This is a temporary workaround.
		 *
		 * Default: false
		 */
		bool forgetPassword;

		/**
		 * Use XEP-0198 acks in the stream when available.
		 * Default: true
		 */
		bool useAcks;

		/**
		 * Use Single Sign On.
		 * Default: false
		 */
		bool singleSignOn;

		/**
		 * The hostname to connect to.
		 * Leave this empty for standard XMPP connection, based on the JID domain.
		 */
		std::string manualHostname;

		/**
		 * The port to connect to.
		 * Leave this to -1 to use the port discovered by SRV lookups, and 5222 as a
		 * fallback.
		 */
		int manualPort;

		/**
		 * The type of proxy to use for connecting to the XMPP
		 * server.
		 */
		ProxyType proxyType;

		/**
		 * Override the system-configured proxy hostname.
		 */
		std::string manualProxyHostname;

		/**
		 * Override the system-configured proxy port.
		 */
		int manualProxyPort;

		/**
		 * If non-empty, use BOSH instead of direct TCP, with the given URL.
		 * Default: empty (no BOSH)
		 */
		URL boshURL;

		/**
		 * If non-empty, BOSH connections will try to connect over this HTTP CONNECT
		 * proxy instead of directly.
		 * Default: empty (no proxy)
		 */
		URL boshHTTPConnectProxyURL;

		/**
		 * If this and matching Password are non-empty, BOSH connections over
		 * HTTP CONNECT proxies will use these credentials for proxy access.
		 * Default: empty (no authentication needed by the proxy)
		 */
		SafeString boshHTTPConnectProxyAuthID;
		SafeString boshHTTPConnectProxyAuthPassword;

		/**
		 * This can be initialized with a custom HTTPTrafficFilter, which allows HTTP CONNECT
		 * proxy initialization to be customized.
		 */
		boost::shared_ptr<HTTPTrafficFilter> httpTrafficFilter;

		/**
		 * Options passed to the TLS stack
		 */
		TLSOptions tlsOptions;
	};
}
