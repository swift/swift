/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_AuthRequestParser_H
#define SWIFTEN_AuthRequestParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class AuthRequestParser : public GenericElementParser<AuthRequest> {
		public:
			AuthRequestParser();

			virtual void handleStartElement(const String&, const String& ns, const AttributeMap&);
			virtual void handleEndElement(const String&, const String& ns);
			virtual void handleCharacterData(const String&);

		private:
			String text_;
			int depth_;
	};
}

#endif
