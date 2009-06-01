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
