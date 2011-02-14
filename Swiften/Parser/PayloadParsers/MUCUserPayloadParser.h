/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Elements/MUCUserPayload.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class MUCUserPayloadParser : public GenericPayloadParser<MUCUserPayload> {
		public:
			MUCUserPayloadParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);
			MUCOccupant::Role parseRole(const std::string& itemString) const;
			MUCOccupant::Affiliation parseAffiliation(const std::string& statusString) const;
		private:
			enum Level { 
				TopLevel = 0, 
				ItemLevel = 1
			};
			int level;
	};
}
