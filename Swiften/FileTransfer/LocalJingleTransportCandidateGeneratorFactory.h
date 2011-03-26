/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class LocalJingleTransportCandidateGenerator;

	class LocalJingleTransportCandidateGeneratorFactory {
		public:
			virtual ~LocalJingleTransportCandidateGeneratorFactory();

			virtual LocalJingleTransportCandidateGenerator* createCandidateGenerator() = 0;
	};
}
