/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PAYLOADPARSERFACTORY_H
#define SWIFTEN_PAYLOADPARSERFACTORY_H

#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class String;
	class PayloadParser;

	class PayloadParserFactory {
		public:
			virtual ~PayloadParserFactory();

			virtual bool canParse(const String& element, const String& ns, const AttributeMap& attributes) const = 0;
			virtual PayloadParser* createPayloadParser() = 0;
	};
}

#endif
