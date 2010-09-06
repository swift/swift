/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <map>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	class StanzaChannel;
	class CapsProvider;

	class EntityCapsManager : public boost::bsignals::trackable { 
		public:
			EntityCapsManager(CapsProvider*, StanzaChannel*);

			DiscoInfo::ref getCaps(const JID&) const;

			boost::signal<void (const JID&)> onCapsChanged;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleCapsAvailable(const String&);

		private:
			CapsProvider* capsProvider;
			std::map<JID, String> caps;
	};
}
