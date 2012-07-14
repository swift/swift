/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/VCard.h>

namespace Swift {
	class JID;

	class SWIFTEN_API VCardStorage {
		public:
			virtual ~VCardStorage();

			virtual VCard::ref getVCard(const JID& jid) const = 0;
			virtual void setVCard(const JID&, VCard::ref) = 0;

			virtual std::string getPhotoHash(const JID&) const;
	};
}
