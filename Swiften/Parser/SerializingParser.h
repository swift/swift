/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SerializingParser_H
#define SWIFTEN_SerializingParser_H

#include "Swiften/Base/String.h"
#include "Swiften/Parser/AttributeMap.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class SerializingParser {
		public:
			SerializingParser();

			void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes);
			void handleEndElement(const String& element, const String& ns);
			void handleCharacterData(const String& data);

			String getResult() const;

		private:
			std::vector< boost::shared_ptr<XMLElement> > elementStack_;
			boost::shared_ptr<XMLElement> rootElement_;
	};
}

#endif
