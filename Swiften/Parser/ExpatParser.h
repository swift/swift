/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <expat.h>
#include <boost/noncopyable.hpp>

#include <Swiften/Parser/XMLParser.h>

namespace Swift {
	class ExpatParser : public XMLParser, public boost::noncopyable {
		public:
			ExpatParser(XMLParserClient* client);
			~ExpatParser();

			bool parse(const std::string& data);

			XML_Parser getParser() {
				return parser_;
			}

		private:
			XML_Parser parser_;
	};
}
