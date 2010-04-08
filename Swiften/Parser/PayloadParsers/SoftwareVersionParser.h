/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SoftwareVersionParser_H
#define SWIFTEN_SoftwareVersionParser_H

#include "Swiften/Elements/SoftwareVersion.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class SoftwareVersionParser : public GenericPayloadParser<SoftwareVersion> {
		public:
			SoftwareVersionParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			int level_;
			String currentText_;
	};
}

#endif
