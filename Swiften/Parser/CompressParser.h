/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/CompressRequest.h>

namespace Swift {
	class CompressParser : public GenericElementParser<CompressRequest> {
		public:
			CompressParser();

		private:
			void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
			void handleEndElement(const std::string& element, const std::string& ns);
			void handleCharacterData(const std::string& data);

		private:
			int currentDepth_;
			std::string currentText_;
			bool inMethod_;
	};
}
