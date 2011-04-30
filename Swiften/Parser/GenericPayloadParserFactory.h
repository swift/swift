/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <string>

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
			GenericPayloadParserFactory(const std::string& tag, const std::string& xmlns = "") : tag_(tag), xmlns_(xmlns) {}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return (tag_.empty() ? true : element == tag_) && (xmlns_.empty() ? true : xmlns_ == ns);
			}

			virtual PayloadParser* createPayloadParser() {
				return new PARSER_TYPE();
			}

		private:
			std::string tag_;
			std::string xmlns_;
	};
}
