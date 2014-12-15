/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Status.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class StatusParser : public GenericPayloadParser<Status> {
		public:
			StatusParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			int level_;
			std::string text_;
	};
}
