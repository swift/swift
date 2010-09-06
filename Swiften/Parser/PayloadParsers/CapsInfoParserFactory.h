/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/CapsInfoParser.h"

namespace Swift {
	class CapsInfoParserFactory : public GenericPayloadParserFactory<CapsInfoParser> {
		public:
			CapsInfoParserFactory() : GenericPayloadParserFactory<CapsInfoParser>("c", "http://jabber.org/protocol/caps") {}
	};
}
