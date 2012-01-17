/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/URL.h>
#include <Swiften/Base/SafeString.h>

namespace Swift {
	struct ClientOptions {
		enum UseTLS {
			NeverUseTLS,
			UseTLSWhenAvailable,
			RequireTLS
		};

		ClientOptions() : useStreamCompression(true), useTLS(UseTLSWhenAvailable), allowPLAINWithoutTLS(false), useStreamResumption(false), forgetPassword(false), useAcks(true), boshHTTPConnectProxyAuthID(""), boshHTTPConnectProxyAuthPassword("") {
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
	};
}
