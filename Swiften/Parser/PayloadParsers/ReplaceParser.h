/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Replace.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class ReplaceParser : public GenericPayloadParser<Replace> {
		public:
			ReplaceParser();
			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			int level_;
	};
}
