/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Stanza.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class StanzaAckResponder {
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
