/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PubSubErrorParser.h>

namespace Swift {
	class PubSubErrorParserFactory : public PayloadParserFactory {
		public:
			PubSubErrorParserFactory() {
			}
			~PubSubErrorParserFactory();

			virtual bool canParse(const std::string&, const std::string& ns, const AttributeMap&) const {
				return ns == "http://jabber.org/protocol/pubsub#errors";
			}

			virtual PayloadParser* createPayloadParser() {
				return new PubSubErrorParser();
			}
	};
}


