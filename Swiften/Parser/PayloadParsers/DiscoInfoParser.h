/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_DiscoInfoParser_H
#define SWIFTEN_DiscoInfoParser_H

#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class DiscoInfoParser : public GenericPayloadParser<DiscoInfo> {
		public:
			DiscoInfoParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			int level_;
	};
}

#endif
