/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class StanzaChannel;
	class VCardManager;
	class VCardUpdateAvatarManager;

	class AvatarManager {
		public:
			AvatarManager(VCardManager*, StanzaChannel*, AvatarStorage*, MUCRegistry* = NULL);
			virtual ~AvatarManager();

			virtual boost::filesystem::path getAvatarPath(const JID&) const;

		public:
			boost::signal<void (const JID&, const String& /*hash*/)> onAvatarChanged;

		private:
			VCardUpdateAvatarManager* vcardUpdateAvatarManager;
	};
}
