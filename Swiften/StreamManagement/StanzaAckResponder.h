/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class SWIFTEN_API StanzaAckResponder {
		public:
			StanzaAckResponder();

			void handleStanzaReceived();
			void handleAckRequestReceived();

		public:
			boost::signal<void (unsigned int /* handledStanzaCount */)> onAck;

		private:
			friend class StanzaAckResponderTest;
			unsigned int handledStanzasCount;
	};

}
