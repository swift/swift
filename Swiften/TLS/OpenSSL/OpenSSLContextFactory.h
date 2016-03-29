/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
	class OpenSSLContextFactory : public TLSContextFactory {
		public:
			bool canCreate() const;
			virtual TLSContext* createTLSContext(const TLSOptions& tlsOptions);

			// Not supported
			virtual void setCheckCertificateRevocation(bool b);
			virtual void setDisconnectOnCardRemoval(bool b);
	};
}
