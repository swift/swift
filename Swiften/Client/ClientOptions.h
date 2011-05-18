/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

struct ClientOptions {
	enum UseTLS {
		NeverUseTLS,
		UseTLSWhenAvailable
	};

	ClientOptions() : useStreamCompression(true), useTLS(UseTLSWhenAvailable), useStreamResumption(false), forgetPassword(false) {
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
};

