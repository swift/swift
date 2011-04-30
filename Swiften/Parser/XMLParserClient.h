/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef XMLPARSERCLIENT_H
#define XMLPARSERCLIENT_H

#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
	

	class XMLParserClient {
		public:
			virtual ~XMLParserClient();

			virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) = 0;
			virtual void handleEndElement(const std::string& element, const std::string& ns) = 0;
			virtual void handleCharacterData(const std::string& data) = 0;
	};
}

#endif
