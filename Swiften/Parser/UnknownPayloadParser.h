/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
	class UnknownPayloadParser : public PayloadParser {
		public:
			UnknownPayloadParser() {}

			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {}
			virtual void handleEndElement(const std::string&, const std::string&) {}
			virtual void handleCharacterData(const std::string&) {}

			virtual boost::shared_ptr<Payload> getPayload() const {
				return boost::shared_ptr<Payload>();
			}
	};
}
