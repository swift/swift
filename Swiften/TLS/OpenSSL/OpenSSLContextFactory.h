/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/TLSContextFactory.h>

#include <cassert>

namespace Swift {
	class OpenSSLContextFactory : public TLSContextFactory {
		public:
			bool canCreate() const;
			virtual TLSContext* createTLSContext();

			// Not supported
			virtual void setCheckCertificateRevocation(bool b);
	};
}
