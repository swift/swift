#pragma once

#include <vector>

#include "Swiften/Client/StanzaChannel.h"

namespace Swift {
	class DummyStanzaChannel : public StanzaChannel {
		public:
			DummyStanzaChannel() {}

			virtual void sendStanza(boost::shared_ptr<Stanza> stanza) {
				sentStanzas_.push_back(stanza);
			}

			virtual void sendIQ(boost::shared_ptr<IQ> iq) {
				sentStanzas_.push_back(iq);
			}

			virtual void sendMessage(boost::shared_ptr<Message> message) {
				sentStanzas_.push_back(message);
			}

			virtual void sendPresence(boost::shared_ptr<Presence> presence) {
				sentStanzas_.push_back(presence);
			}

			virtual String getNewIQID() {
				return "test-id";
			}
			
			virtual bool isAvailable() {
				return true;
			}

			std::vector<boost::shared_ptr<Stanza> > sentStanzas_;
	};
}
