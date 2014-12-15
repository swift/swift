/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
