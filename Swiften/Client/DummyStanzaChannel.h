/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include "Swiften/Client/StanzaChannel.h"

namespace Swift {
	class DummyStanzaChannel : public StanzaChannel {
		public:
			DummyStanzaChannel() : available_(true) {}

			virtual void sendStanza(boost::shared_ptr<Stanza> stanza) {
				sentStanzas.push_back(stanza);
			}

			void setAvailable(bool available) {
				available_ = available;
				onAvailableChanged(available);
			}

			virtual void sendIQ(boost::shared_ptr<IQ> iq) {
				sentStanzas.push_back(iq);
			}

			virtual void sendMessage(boost::shared_ptr<Message> message) {
				sentStanzas.push_back(message);
			}

			virtual void sendPresence(boost::shared_ptr<Presence> presence) {
				sentStanzas.push_back(presence);
			}

			virtual String getNewIQID() {
				return "test-id";
			}
			
			virtual bool isAvailable() {
				return available_;
			}

			virtual bool getStreamManagementEnabled() const {
				return false;
			}

			template<typename T> bool isRequestAtIndex(size_t index, const JID& jid, IQ::Type type) {
				if (index >= sentStanzas.size()) {
					return false;
				}
				boost::shared_ptr<IQ> iqStanza = boost::dynamic_pointer_cast<IQ>(sentStanzas[index]);
				return iqStanza && iqStanza->getType() == type && iqStanza->getTo() == jid && iqStanza->getPayload<T>();
			}

			std::vector<boost::shared_ptr<Stanza> > sentStanzas;
			bool available_;
	};
}
