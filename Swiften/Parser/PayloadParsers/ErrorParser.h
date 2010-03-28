#ifndef SWIFTEN_ErrorParser_H
#define SWIFTEN_ErrorParser_H

#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class ErrorParser : public GenericPayloadParser<ErrorPayload> {
		public:
			ErrorParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			int level_;
			String currentText_;
	};
}

#endif
