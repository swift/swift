/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/SerializingParser.h>

namespace Swift {
	class SerializingParser;

	class RawXMLPayloadParser : public GenericPayloadParser<RawXMLPayload> {
		public:
			RawXMLPayloadParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			int level_;
			SerializingParser serializingParser_;
	};
}
