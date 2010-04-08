/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PriorityParser_H
#define SWIFTEN_PriorityParser_H

#include "Swiften/Elements/Priority.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class PriorityParser : public GenericPayloadParser<Priority> {
		public:
			PriorityParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level_;
			String text_;
	};
}

#endif
