/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/MUCItem.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
	class MUCItemParser  {
		public:
			static MUCItem itemFromTree(ParserElement::ref root);
		private:
			static boost::optional<MUCOccupant::Role> parseRole(const std::string& itemString);
			static boost::optional<MUCOccupant::Affiliation> parseAffiliation(const std::string& statusString);
	};
}
