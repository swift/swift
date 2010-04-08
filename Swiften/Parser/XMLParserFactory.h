/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
