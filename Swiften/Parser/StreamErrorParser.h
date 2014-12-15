/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/StreamError.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
	class StreamErrorParser : public GenericElementParser<StreamError> {
		public:
			StreamErrorParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				ElementLevel = 1
			};
			int level;
			std::string currentText;
	};
}
