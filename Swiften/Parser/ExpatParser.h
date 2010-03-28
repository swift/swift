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

			bool parse(const String& data);

		private:
			XML_Parser parser_;
	};
}

#endif
