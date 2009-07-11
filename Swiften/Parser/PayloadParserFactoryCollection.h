#ifndef SWIFTEN_PAYLOADPARSERFACTORYCOLLECTION_H
#define SWIFTEN_PAYLOADPARSERFACTORYCOLLECTION_H

#include <vector>

#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class PayloadParserFactory;
	class String;

	class PayloadParserFactoryCollection {
		public:
			PayloadParserFactoryCollection();

			void addFactory(PayloadParserFactory* factory);
			void removeFactory(PayloadParserFactory* factory);
			void setDefaultFactory(PayloadParserFactory* factory);

			PayloadParserFactory* getPayloadParserFactory(const String& element, const String& ns, const AttributeMap& attributes);

		private:
			std::vector<PayloadParserFactory*> factories_;
			PayloadParserFactory* defaultFactory_;
	};
}

#endif
