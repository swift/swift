/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/TLS/TLSContextFactory.h"

namespace Swift {
	class PlatformTLSContextFactory : public TLSContextFactory {
		public:
			PlatformTLSContextFactory();
			~PlatformTLSContextFactory();

			bool canCreate() const;
			virtual TLSContext* createTLSContext();

		private:
			TLSContextFactory* factory;
	};
}
