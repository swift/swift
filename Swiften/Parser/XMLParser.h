/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_XMLParser_H
#define SWIFTEN_XMLParser_H

namespace Swift {
	class String;
	class XMLParserClient;

	class XMLParser {
		public:
			XMLParser(XMLParserClient* client);
			virtual ~XMLParser();

			virtual bool parse(const String& data) = 0;

		protected:
			XMLParserClient* getClient() const {
				return client_;
			}

		private:
			XMLParserClient* client_;
	};
}

#endif
