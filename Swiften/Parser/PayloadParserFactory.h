/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
	
	class PayloadParser;

	/**
	 * A factory for PayloadParsers.
	 */
	class PayloadParserFactory {
		public:
			virtual ~PayloadParserFactory();

			/**
			 * Checks whether this factory can parse the given top-level element in the given namespace (with the given attributes).
			 */
			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap& attributes) const = 0;

			/**
			 * Creates a new payload parser.
			 */
			virtual PayloadParser* createPayloadParser() = 0;
	};
}
