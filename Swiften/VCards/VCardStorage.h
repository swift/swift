/*
 * Copyright (c) 2010-2013 Remko Tronçon
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
	class CryptoProvider;

	class SWIFTEN_API VCardStorage {
		public:
			VCardStorage(CryptoProvider*);
			virtual ~VCardStorage();

			virtual VCard::ref getVCard(const JID& jid) const = 0;
			virtual void setVCard(const JID&, VCard::ref) = 0;

			virtual std::string getPhotoHash(const JID&) const;

		private:
			CryptoProvider* crypto;
	};
}
