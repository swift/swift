/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
	class XMLParser;
	class XMPPParserClient;
	class XMLParserFactory;	
	class ElementParser;
	class PayloadParserFactoryCollection;

	class XMPPParser : public XMLParserClient, boost::noncopyable {
		public:
			XMPPParser(
					XMPPParserClient* parserClient, 
					PayloadParserFactoryCollection* payloadParserFactories,
					XMLParserFactory* xmlParserFactory);
			~XMPPParser();

			bool parse(const std::string&);

		private:
			virtual void handleStartElement(
					const std::string& element, 
					const std::string& ns, 
					const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string& ns);
			virtual void handleCharacterData(const std::string& data);

			ElementParser* createElementParser(const std::string& element, const std::string& xmlns);

		private:
			XMLParser* xmlParser_;
			XMPPParserClient* client_;
			PayloadParserFactoryCollection* payloadParserFactories_;
			enum Level {
				TopLevel = 0,
				StreamLevel = 1,
				ElementLevel = 2
			};
			int level_;
			ElementParser* currentElementParser_;
			bool parseErrorOccurred_;
	};
}
