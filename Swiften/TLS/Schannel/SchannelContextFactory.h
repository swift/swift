/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include "Swiften/TLS/TLSContextFactory.h"

namespace Swift {
	class SchannelContextFactory : public TLSContextFactory {
		public:
			bool canCreate() const;
			virtual TLSContext* createTLSContext();
	};
}
