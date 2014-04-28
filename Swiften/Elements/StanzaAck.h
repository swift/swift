/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	class StanzaAck : public ToplevelElement {
		public:
			typedef boost::shared_ptr<StanzaAck> ref;

			StanzaAck() : valid(false), handledStanzasCount(0) {}
			StanzaAck(unsigned int handledStanzasCount) : valid(true), handledStanzasCount(handledStanzasCount) {}
			virtual ~StanzaAck();

			unsigned int getHandledStanzasCount() const {
				return handledStanzasCount;
			}

			void setHandledStanzasCount(int i);

			bool isValid() const {
				return valid;
			}

		private:
			bool valid;
			unsigned int handledStanzasCount;
	};
}
