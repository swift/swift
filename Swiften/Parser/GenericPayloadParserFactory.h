/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_GENERICPAYLOADPARSERFACTORY_H
#define SWIFTEN_GENERICPAYLOADPARSERFACTORY_H

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Base/String.h"

namespace Swift {

	template<typename PARSER_TYPE>
	class GenericPayloadParserFactory : public PayloadParserFactory {
		public:
			GenericPayloadParserFactory(const String& tag, const String& xmlns = "") : tag_(tag), xmlns_(xmlns) {}

			virtual bool canParse(const String& element, const String& ns, const AttributeMap&) const {
				return element == tag_ && (xmlns_.isEmpty() ? true : xmlns_ == ns);
			}

			virtual PayloadParser* createPayloadParser() {
				return new PARSER_TYPE();
			}

		private:
			String tag_;
			String xmlns_;
	};
}

#endif
