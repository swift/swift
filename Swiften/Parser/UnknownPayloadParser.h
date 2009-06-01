#ifndef SWIFTEN_UNKNOWNPAYLOADPARSER_H
#define SWIFTEN_UNKNOWNPAYLOADPARSER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Parser/PayloadParser.h"

namespace Swift {
	class String;

	class UnknownPayloadParser : public PayloadParser {
		public:
			UnknownPayloadParser() {}

			virtual void handleStartElement(const String&, const String&, const AttributeMap&) {}
			virtual void handleEndElement(const String&, const String&) {}
			virtual void handleCharacterData(const String&) {}

			virtual boost::shared_ptr<Payload> getPayload() const {
				return boost::shared_ptr<Payload>();
			}
	};
}

#endif
