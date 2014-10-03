/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <map>

#include "Swiften/VCards/VCardStorage.h"

namespace Swift {
	class CryptoProvider;

	class VCardFileStorage : public VCardStorage {
		public:
			VCardFileStorage(boost::filesystem::path dir, CryptoProvider* crypto);

			virtual VCard::ref getVCard(const JID& jid) const;
			virtual boost::posix_time::ptime getVCardWriteTime(const JID& jid) const;
			virtual void setVCard(const JID& jid, VCard::ref v);

			virtual std::string getPhotoHash(const JID&) const;

		private:
			boost::filesystem::path getVCardPath(const JID&) const;

			std::string getAndUpdatePhotoHash(const JID& jid, VCard::ref vcard) const;
			void savePhotoHashes() const;

		private:
			boost::filesystem::path vcardsPath;
			CryptoProvider* crypto;
			boost::filesystem::path cacheFile;
			typedef std::map<JID, std::string> PhotoHashMap;
			mutable PhotoHashMap photoHashes;
			std::map<JID, boost::posix_time::ptime> vcardWriteTimes;
	};
}
