/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Parser/XMLParser.h>

namespace Swift {
	class ExpatParser : public XMLParser, public boost::noncopyable {
		public:
			ExpatParser(XMLParserClient* client);
			~ExpatParser();

			bool parse(const std::string& data);

			void stopParser();

		private:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
