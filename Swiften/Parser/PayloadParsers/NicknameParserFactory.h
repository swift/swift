/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/NicknameParser.h"

namespace Swift {
	class NicknameParserFactory : public GenericPayloadParserFactory<NicknameParser> {
		public:
			NicknameParserFactory() : GenericPayloadParserFactory<NicknameParser>("nick", "http://jabber.org/protocol/nick") {}
	};
}
