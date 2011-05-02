/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class SerializingParser;

	class RosterParser : public GenericPayloadParser<RosterPayload> {
		public:
			RosterParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				ItemLevel = 2
			};
			int level_;
			bool inItem_;
			RosterItemPayload currentItem_;
			std::string currentText_;
			SerializingParser* unknownContentParser_;
	};
}
