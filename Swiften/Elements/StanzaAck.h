/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Element.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class StanzaAck : public Element, public Shared<StanzaAck> {
		public:
			StanzaAck() : handledStanzasCount(-1) {}

			int getHandledStanzasCount() const {
				return handledStanzasCount;
			}

			void setHandledStanzasCount(int i) {
				handledStanzasCount = i;
			}

			bool isValid() const {
				return handledStanzasCount != -1;
			}

		private:
			int handledStanzasCount;
	};
}
