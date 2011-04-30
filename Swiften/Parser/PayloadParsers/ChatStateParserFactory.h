/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ChatStateParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class ChatStateParserFactory : public PayloadParserFactory {
		public:
			ChatStateParserFactory() {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return ns == "http://jabber.org/protocol/chatstates" && 
					(element == "active" || element == "composing" 
					 || element == "paused" || element == "inactive" || element == "gone");
			}

			virtual PayloadParser* createPayloadParser() {
				return new ChatStateParser();
			}

	};
}
