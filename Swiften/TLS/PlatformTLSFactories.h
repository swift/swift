/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
	class TLSContextFactory;
	class CertificateFactory;

	class SWIFTEN_API PlatformTLSFactories {
		public:
			PlatformTLSFactories();
			~PlatformTLSFactories();

			TLSContextFactory* getTLSContextFactory() const;
			CertificateFactory* getCertificateFactory() const;

		private:
			TLSContextFactory* contextFactory;
			CertificateFactory* certificateFactory;
	};
}
