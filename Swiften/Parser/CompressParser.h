/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_CompressParser_H
#define SWIFTEN_CompressParser_H

#include "Swiften/Base/String.h"
#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/CompressRequest.h"

namespace Swift {
	class CompressParser : public GenericElementParser<CompressRequest> {
		public:
			CompressParser();

		private:
			void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes);
			void handleEndElement(const String& element, const String& ns);
			void handleCharacterData(const String& data);

		private:
			int currentDepth_;
			String currentText_;
			bool inMethod_;
	};
}

#endif
