/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Element.h>


namespace Swift {
	class StanzaAck : public Element {
		public:
			typedef boost::shared_ptr<StanzaAck> ref;

			StanzaAck() : valid(false), handledStanzasCount(0) {}
			StanzaAck(unsigned int handledStanzasCount) : valid(true), handledStanzasCount(handledStanzasCount) {}

			unsigned int getHandledStanzasCount() const {
				return handledStanzasCount;
			}

			void setHandledStanzasCount(int i) {
				handledStanzasCount = i;
				valid = true;
			}

			bool isValid() const {
				return valid;
			}

		private:
			bool valid;
			unsigned int handledStanzasCount;
	};
}
