#ifndef SWIFTEN_FULLPAYLOADPARSERFACTORYCOLLECTION_H
#define SWIFTEN_FULLPAYLOADPARSERFACTORYCOLLECTION_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Parser/PayloadParserFactoryCollection.h"
#include "Swiften/Parser/PayloadParserFactory.h"

namespace Swift {
	class FullPayloadParserFactoryCollection : public PayloadParserFactoryCollection {
		public:
			FullPayloadParserFactoryCollection();
			~FullPayloadParserFactoryCollection();

		private:
			std::vector< boost::shared_ptr<PayloadParserFactory> > factories_;
	};
}

#endif
