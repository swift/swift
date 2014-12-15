/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
