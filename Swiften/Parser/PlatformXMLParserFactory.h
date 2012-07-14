/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/XMLParserFactory.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API PlatformXMLParserFactory : public XMLParserFactory {
		public:
			PlatformXMLParserFactory();

			virtual XMLParser* createXMLParser(XMLParserClient*);
	};
}
