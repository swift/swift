/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h"

namespace Swift {
	class MUCUserPayloadParserFactory : public GenericPayloadParserFactory<MUCUserPayloadParser> {
		public:
			MUCUserPayloadParserFactory() : GenericPayloadParserFactory<MUCUserPayloadParser>("x", "http://jabber.org/protocol/muc#user") {}
	};
}
