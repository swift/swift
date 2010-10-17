/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/BytestreamsParser.h"

namespace Swift {
	class BytestreamsParserFactory : public GenericPayloadParserFactory<BytestreamsParser> {
		public:
			BytestreamsParserFactory() : GenericPayloadParserFactory<BytestreamsParser>("query", "http://jabber.org/protocol/bytestreams") {}
	};
}
