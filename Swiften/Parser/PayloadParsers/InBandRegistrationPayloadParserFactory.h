/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/InBandRegistrationPayloadParser.h"

namespace Swift {
	class InBandRegistrationPayloadParserFactory : public GenericPayloadParserFactory<InBandRegistrationPayloadParser> {
		public:
			InBandRegistrationPayloadParserFactory() : GenericPayloadParserFactory<InBandRegistrationPayloadParser>("query", "jabber:iq:register") {}
	};
}
