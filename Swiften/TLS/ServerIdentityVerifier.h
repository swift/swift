/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
	class ServerIdentityVerifier {
		public:
			ServerIdentityVerifier(const JID& jid);

			bool certificateVerifies(Certificate::ref);

		private:
			bool matchesDomain(const std::string&) const ;
			bool matchesAddress(const std::string&) const;

		private:
			std::string domain;
			std::string encodedDomain;
	};
}
