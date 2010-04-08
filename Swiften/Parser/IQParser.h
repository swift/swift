/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_IQParser_H
#define SWIFTEN_IQParser_H

#include "Swiften/Parser/GenericStanzaParser.h"
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQParser : public GenericStanzaParser<IQ> {
		public:
			IQParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStanzaAttributes(const AttributeMap&);
	};
}

#endif
