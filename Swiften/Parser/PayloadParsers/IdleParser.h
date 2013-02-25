/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Idle.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class IdleParser : public GenericPayloadParser<Idle> {
		public:
			IdleParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			int level_;
	};
}
