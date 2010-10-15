/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/ComponentHandshake.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class ComponentHandshakeParser : public GenericElementParser<ComponentHandshake> {
		public:
			ComponentHandshakeParser();

			virtual void handleStartElement(const String&, const String& ns, const AttributeMap&);
			virtual void handleEndElement(const String&, const String& ns);
			virtual void handleCharacterData(const String&);

		private:
			int depth;
			String text;
	};
}
