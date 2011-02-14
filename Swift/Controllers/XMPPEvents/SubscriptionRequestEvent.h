/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cassert>

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/XMPPEvents/StanzaEvent.h"
#include <string>
#include "Swiften/JID/JID.h"

namespace Swift {
	class SubscriptionRequestEvent : public StanzaEvent {
		public:
			SubscriptionRequestEvent(const JID& jid, const std::string& reason) : jid_(jid), reason_(reason){};
			virtual ~SubscriptionRequestEvent(){};
			const JID& getJID() const {return jid_;};
			const std::string& getReason() const {return reason_;};
			boost::signal<void()> onAccept;
			boost::signal<void()> onDecline;
			void accept() {
				onAccept();
				conclude();
			};

			void decline() {
				onDecline();
				conclude();
			};

			void defer() {
				conclude();
			}

		private:
			JID jid_;
			std::string reason_;
	};
}

