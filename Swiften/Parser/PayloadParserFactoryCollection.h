/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
	class PayloadParserFactory;

	class PayloadParserFactoryCollection {
		public:
			PayloadParserFactoryCollection();

			void addFactory(PayloadParserFactory* factory);
			void removeFactory(PayloadParserFactory* factory);
			void setDefaultFactory(PayloadParserFactory* factory);

			PayloadParserFactory* getPayloadParserFactory(const std::string& element, const std::string& ns, const AttributeMap& attributes);

		private:
			std::vector<PayloadParserFactory*> factories_;
			PayloadParserFactory* defaultFactory_;
	};
}
