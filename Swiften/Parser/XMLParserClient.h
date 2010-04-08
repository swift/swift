/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef XMLPARSERCLIENT_H
#define XMLPARSERCLIENT_H

#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class String;

	class XMLParserClient {
		public:
			virtual ~XMLParserClient();

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) = 0;
			virtual void handleEndElement(const String& element, const String& ns) = 0;
			virtual void handleCharacterData(const String& data) = 0;
	};
}

#endif
