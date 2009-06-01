#ifndef SWIFTEN_SecurityLabelParser_H
#define SWIFTEN_SecurityLabelParser_H

#include "Swiften/Elements/SecurityLabel.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class SerializingParser;

	class SecurityLabelParser : public GenericPayloadParser<SecurityLabel> {
		public:
			SecurityLabelParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				DisplayMarkingOrLabelLevel = 2,
				SecurityLabelLevel = 3
			};
			int level_;
			SerializingParser* labelParser_;
			String currentText_;
	};
}

#endif
