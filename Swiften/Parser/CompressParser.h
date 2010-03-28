#ifndef SWIFTEN_CompressParser_H
#define SWIFTEN_CompressParser_H

#include "Swiften/Base/String.h"
#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/CompressRequest.h"

namespace Swift {
	class CompressParser : public GenericElementParser<CompressRequest> {
		public:
			CompressParser();

		private:
			void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes);
			void handleEndElement(const String& element, const String& ns);
			void handleCharacterData(const String& data);

		private:
			int currentDepth_;
			String currentText_;
			bool inMethod_;
	};
}

#endif
