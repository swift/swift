#ifndef SWIFTEN_SecurityLabelsCatalogParser_H
#define SWIFTEN_SecurityLabelsCatalogParser_H

#include "Swiften/Elements/SecurityLabelsCatalog.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class SecurityLabelParserFactory;
	class SecurityLabelParser;

	class SecurityLabelsCatalogParser : public GenericPayloadParser<SecurityLabelsCatalog> {
		public:
			SecurityLabelsCatalogParser();
			~SecurityLabelsCatalogParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				LabelLevel = 2
			};
			int level_;
			SecurityLabelParserFactory* labelParserFactory_;
			SecurityLabelParser* labelParser_;
	};
}

#endif
