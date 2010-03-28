#ifndef SWIFTEN_PAYLOADPARSER_H
#define SWIFTEN_PAYLOADPARSER_H

#include <boost/shared_ptr.hpp>
#include "Swiften/Parser/AttributeMap.h"

#include "Swiften/Elements/Payload.h"

namespace Swift {
	class String;

	class PayloadParser {
		public:
			virtual ~PayloadParser();

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) = 0;
			virtual void handleEndElement(const String& element, const String& ns) = 0;
			virtual void handleCharacterData(const String& data) = 0;

			virtual boost::shared_ptr<Payload> getPayload() const = 0;
	};
}

#endif
