/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/SecurityLabelParser.h>

namespace Swift {
	class SecurityLabelParserFactory : public GenericPayloadParserFactory<SecurityLabelParser> {
		public:
			SecurityLabelParserFactory() : GenericPayloadParserFactory<SecurityLabelParser>("securitylabel", "urn:xmpp:sec-label:0") {}
	};
}
