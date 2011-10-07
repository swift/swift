/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <libxml/parser.h>
#include <boost/noncopyable.hpp>

#include <Swiften/Parser/XMLParser.h>

namespace Swift {
	/**
	 * Warning: This constructor is not thread-safe, because it depends on global state to
	 * check whether it is initialized.
	 */
	class LibXMLParser : public XMLParser, public boost::noncopyable {
		public:
			LibXMLParser(XMLParserClient* client);
			~LibXMLParser();

			bool parse(const std::string& data);

		private:
			static bool initialized;
			xmlSAXHandler handler_;
			xmlParserCtxtPtr context_;
	};
}
