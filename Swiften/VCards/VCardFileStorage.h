/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include "Swiften/VCards/VCardStorage.h"

namespace Swift {
	class VCardFileStorage : public VCardStorage {
		public:
			VCardFileStorage(boost::filesystem::path dir);

			virtual VCard::ref getVCard(const JID& jid) const;
			virtual void setVCard(const JID& jid, VCard::ref v);

		private:
			boost::filesystem::path getVCardPath(const JID&) const;

		private:
			boost::filesystem::path vcardsPath;
	};
}
