/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <deque>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class SWIFTEN_API StanzaAckRequester {
		public:
			StanzaAckRequester();

			void handleStanzaSent(boost::shared_ptr<Stanza> stanza);
			void handleAckReceived(unsigned int handledStanzasCount);

		public:
			boost::signal<void ()> onRequestAck;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaAcked;

		private:
			friend class StanzaAckRequesterTest;
			unsigned int lastHandledStanzasCount;
			std::deque<boost::shared_ptr<Stanza> > unackedStanzas;
	};

}
