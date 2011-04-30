/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>

namespace Swift {
	class FullPayloadParserFactoryCollection : public PayloadParserFactoryCollection {
		public:
			FullPayloadParserFactoryCollection();
			~FullPayloadParserFactoryCollection();

		private:
			std::vector< boost::shared_ptr<PayloadParserFactory> > factories_;
			PayloadParserFactory* defaultFactory_;
	};
}
