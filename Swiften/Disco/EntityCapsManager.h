/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Disco/EntityCapsProvider.h>

namespace Swift {
	class StanzaChannel;
	class CapsProvider;

	/**
	 * This class is responsible for gathering and providing
	 * information about capabilities of entities on the network.
	 * This information is provided in the form of service discovery
	 * information.
	 */
	class EntityCapsManager : public EntityCapsProvider, public boost::bsignals::trackable {
		public:
			EntityCapsManager(CapsProvider*, StanzaChannel*);

			/**
			 * Returns the service discovery information of the given JID.
			 */
			DiscoInfo::ref getCaps(const JID&) const;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleCapsAvailable(const std::string&);

		private:
			CapsProvider* capsProvider;
			std::map<JID, std::string> caps;
	};
}
