/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericStanzaParser.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	class SWIFTEN_API PresenceParser : public GenericStanzaParser<Presence> {
		public:
			PresenceParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStanzaAttributes(const AttributeMap&);
	};
}
