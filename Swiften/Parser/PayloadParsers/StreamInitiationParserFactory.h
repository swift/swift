/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StreamInitiationParser.h"

namespace Swift {
	class StreamInitiationParserFactory : public GenericPayloadParserFactory<StreamInitiationParser> {
		public:
			StreamInitiationParserFactory() : GenericPayloadParserFactory<StreamInitiationParser>("si", "http://jabber.org/protocol/si") {}
	};
}
