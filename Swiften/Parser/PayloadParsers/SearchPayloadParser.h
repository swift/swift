/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/SearchPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class FormParserFactory;
	class FormParser;

	class SearchPayloadParser : public GenericPayloadParser<SearchPayload> {
		public:
			SearchPayloadParser();
			~SearchPayloadParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				ItemLevel = 2,
			};
			int level;
			FormParserFactory* formParserFactory;
			FormParser* formParser;
			std::string currentText;
			boost::optional<SearchPayload::Item> currentItem;
	};
}
