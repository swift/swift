#ifndef SWIFTEN_ElementParser_H
#define SWIFTEN_ElementParser_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class ElementParser {
		public:
			virtual ~ElementParser();

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) = 0;
			virtual void handleEndElement(const String& element, const String& ns) = 0;
			virtual void handleCharacterData(const String& data) = 0;

			virtual boost::shared_ptr<Element> getElement() const = 0;
	};
}

#endif
