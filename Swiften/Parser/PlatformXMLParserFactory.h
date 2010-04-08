/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
