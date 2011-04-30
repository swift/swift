/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/StreamInitiation.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class FormParserFactory;
	class FormParser;

	class StreamInitiationParser : public GenericPayloadParser<StreamInitiation> {
		public:
			StreamInitiationParser();
			~StreamInitiationParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				FileOrFeatureLevel = 2,
				FormOrDescriptionLevel = 3,
			};
			int level;
			FormParserFactory* formParserFactory;
			FormParser* formParser;
			bool inFile;
			bool inFeature;
			StreamInitiationFileInfo currentFile;
			std::string currentText;
	};
}
