/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
	class LocalJingleTransportCandidateGenerator;

	class SWIFTEN_API LocalJingleTransportCandidateGeneratorFactory {
		public:
			virtual ~LocalJingleTransportCandidateGeneratorFactory();

			virtual LocalJingleTransportCandidateGenerator* createCandidateGenerator() = 0;
	};
}
