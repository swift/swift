/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Parser/AttributeMap.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class SerializingParser {
		public:
			SerializingParser();

			void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
			void handleEndElement(const std::string& element, const std::string& ns);
			void handleCharacterData(const std::string& data);

			std::string getResult() const;

		private:
			std::vector< boost::shared_ptr<XMLElement> > elementStack_;
			boost::shared_ptr<XMLElement> rootElement_;
	};
}
