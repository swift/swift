/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <set>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "SwifTools/Notifier/Notifier.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Network/Timer.h"

namespace Swift {
	class TimerFactory;
	class StanzaChannel;
	class MUCRegistry;
	class NickResolver;
	class PresenceOracle;

	class PresenceNotifier {
		public:
			PresenceNotifier(StanzaChannel* stanzaChannel, Notifier* notifier, const MUCRegistry* mucRegistry, AvatarManager* avatarManager, NickResolver* nickResolver, const PresenceOracle* presenceOracle, TimerFactory* timerFactory);
			~PresenceNotifier();

			void setInitialQuietPeriodMS(int ms);

			boost::signal<void (const JID&)> onNotificationActivated;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleNotificationActivated(JID jid);
			void handleTimerTick();
			std::string getStatusType(const JID&) const;
			std::string getStatusMessage(const JID&) const;

		private:
			void showNotification(const JID& jid, Notifier::Type type);

		private:
			StanzaChannel* stanzaChannel;
			Notifier* notifier;
			const MUCRegistry* mucRegistry;
			AvatarManager* avatarManager;
			NickResolver* nickResolver;
			const PresenceOracle* presenceOracle;
			TimerFactory* timerFactory;
			boost::shared_ptr<Timer> timer;
			bool justInitialized;
			bool inQuietPeriod;
			std::set<JID> availableUsers;
	};
}

