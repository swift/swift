/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	
	class XMLParserClient;

	class XMLParser {
		public:
			XMLParser(XMLParserClient* client);
			virtual ~XMLParser();

			virtual bool parse(const std::string& data) = 0;

			XMLParserClient* getClient() const {
				return client_;
			}

		private:
			XMLParserClient* client_;
	};
}
