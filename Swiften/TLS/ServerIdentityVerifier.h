/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/TLS/Certificate.h"

namespace Swift {
	class ServerIdentityVerifier {
		public:
			ServerIdentityVerifier(const JID& jid);

			bool certificateVerifies(Certificate::ref);

		private:
			bool matchesDomain(const String&);
			bool matchesAddress(const String&);

		private:
			String domain;
			String encodedDomain;
	};
}
