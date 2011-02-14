/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ExpatParser_H
#define SWIFTEN_ExpatParser_H

#include <expat.h>
#include <boost/noncopyable.hpp>

#include "Swiften/Parser/XMLParser.h"

namespace Swift {
	class ExpatParser : public XMLParser, public boost::noncopyable {
		public:
			ExpatParser(XMLParserClient* client);
			~ExpatParser();

			bool parse(const std::string& data);

		private:
			XML_Parser parser_;
	};
}

#endif
