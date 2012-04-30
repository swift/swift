/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
	class OpenSSLContextFactory : public TLSContextFactory {
		public:
			bool canCreate() const;
			virtual TLSContext* createTLSContext();
	};
}
