/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/StreamError.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
	class StreamErrorParser : public GenericElementParser<StreamError> {
		public:
			StreamErrorParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				ElementLevel = 1
			};
			int level;
			String currentText;
	};
}
