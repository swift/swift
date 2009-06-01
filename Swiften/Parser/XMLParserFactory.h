#ifndef SWIFTEN_XMLParserFactory_H
#define SWIFTEN_XMLParserFactory_H

namespace Swift {
	class XMLParser;
	class XMLParserClient;

	class XMLParserFactory {
		public:
			virtual ~XMLParserFactory();

			virtual XMLParser* createXMLParser(XMLParserClient*) = 0;
	};
}

#endif
