/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/XMLParser.h>

namespace Swift {
	class SWIFTEN_API ExpatParser : public XMLParser, public boost::noncopyable {
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
