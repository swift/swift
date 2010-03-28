#ifndef SWIFTEN_PlatformXMLParserFactory_H
#define SWIFTEN_PlatformXMLParserFactory_H

#include "Swiften/Parser/XMLParserFactory.h"

namespace Swift {
	class PlatformXMLParserFactory : public XMLParserFactory {
		public:
			PlatformXMLParserFactory();

			virtual XMLParser* createXMLParser(XMLParserClient*);
	};
}

#endif
