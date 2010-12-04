/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Base/String.h"

namespace Swift {

	/**
	 * A generic class for PayloadParserFactories that parse a specific payload (given as the template parameter of the class).
	 */
	template<typename PARSER_TYPE>
	class GenericPayloadParserFactory : public PayloadParserFactory {
		public:
			/**
			 * Construct a parser factory that can parse the given top-level tag in the given namespace.
			 */
			GenericPayloadParserFactory(const String& tag, const String& xmlns = "") : tag_(tag), xmlns_(xmlns) {}

			virtual bool canParse(const String& element, const String& ns, const AttributeMap&) const {
				return (tag_.isEmpty() ? true : element == tag_) && (xmlns_.isEmpty() ? true : xmlns_ == ns);
			}

			virtual PayloadParser* createPayloadParser() {
				return new PARSER_TYPE();
			}

		private:
			String tag_;
			String xmlns_;
	};
}
