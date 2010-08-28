/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/StanzaAck.h"

namespace Swift {
	class StanzaAckParser : public GenericElementParser<StanzaAck> {
		public:
			StanzaAckParser();

			virtual void handleStartElement(const String&, const String& ns, const AttributeMap&);
			virtual void handleEndElement(const String&, const String& ns);

		private:
			int depth;
	};
}
