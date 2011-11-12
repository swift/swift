/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/BOSHBodyExtractor.h>

#include <boost/shared_ptr.hpp>

#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMLParserFactory.h>

namespace Swift {

class BOSHBodyParserClient : public XMLParserClient {
	public:
		BOSHBodyParserClient(BOSHBodyExtractor* bodyExtractor) : bodyExtractor(bodyExtractor) {
		}

		virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
			bodyExtractor->body->attributes = attributes;
		}

		virtual void handleEndElement(const std::string&, const std::string&) {
		}

		virtual void handleCharacterData(const std::string&) {
		}

	private:
		BOSHBodyExtractor* bodyExtractor;
};

inline bool isWhitespace(char c) {
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

BOSHBodyExtractor::BOSHBodyExtractor(XMLParserFactory* parserFactory, const ByteArray& data) {
	// Look for the opening body element
	ByteArray::const_iterator i = data.begin();
	while (i < data.end() && isWhitespace(*i)) {
		++i;
	}
	if (std::distance(i, data.end()) < 6 || *i != '<' || *(i+1) != 'b' || *(i+2) != 'o' || *(i+3) != 'd' || *(i+4) != 'y' || !(isWhitespace(*(i+5)) || *(i+5) == '>' || *(i+5) == '/')) {
		return;
	}
	i += 5;

	// Parse until end of element
	bool inSingleQuote = false;
	bool inDoubleQuote = false;
	bool endStartTagSeen = false;
	bool endElementSeen = false;
	for (; i != data.end(); ++i) {
		char c = static_cast<char>(*i);
		if (inSingleQuote) {
			if (c == '\'') {
				inSingleQuote = false;
			}
		}
		else if (inDoubleQuote) {
			if (c == '"') {
					inDoubleQuote = false;
				}
		}
		else if (c == '\'') {
			inSingleQuote = true;
		}
		else if (c == '"') {
			inDoubleQuote = true;
		}
		else if (c == '/') {
			if (i + 1 == data.end() || *(i+1) != '>') {
				return;
			}
			else {
				endElementSeen = true;
				endStartTagSeen = true;
				i += 2;
				break;
			}
		}
		else if (c == '>') {
			endStartTagSeen = true;
			i += 1;
			break;
		}
	}

	if (!endStartTagSeen) {
		return;
	}

	// Look for the end of the element
	ByteArray::const_reverse_iterator j = data.rbegin();
	if (!endElementSeen) {
		while (isWhitespace(*j) && j < data.rend()) {
			++j;
		}

		if (j == data.rend() || *j != '>') {
			return;
		}
		++j;

		while (j < data.rend() && isWhitespace(*j)) {
			++j;
		}

		if (std::distance(j, data.rend()) < 6 || *(j+5) != '<' || *(j+4) != '/' || *(j+3) != 'b' || *(j+2) != 'o' || *(j+1) != 'd' || *j != 'y') {
			return;
		}
		j += 6;
	}

	body = BOSHBody();
	if (!endElementSeen) {
		body->content = std::string(reinterpret_cast<const char*>(vecptr(data) + std::distance(data.begin(), i)), std::distance(i, j.base()));
	}

	// Parse the body element
	BOSHBodyParserClient parserClient(this);
	boost::shared_ptr<XMLParser> parser(parserFactory->createXMLParser(&parserClient));
	if (!parser->parse(std::string(reinterpret_cast<const char*>(vecptr(data)), std::distance(data.begin(), i)))) {
		/* TODO: This needs to be only validating the BOSH <body> element, so that XMPP parsing errors are caught at
		   the correct higher layer */
		body = boost::optional<BOSHBody>();
		return;
	}
}

}
