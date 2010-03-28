#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Element.h"

namespace Swift {
	class String;
	class ProtocolHeader;

	class XMPPParserClient {
		public:
			virtual ~XMPPParserClient();

			virtual void handleStreamStart(const ProtocolHeader&) = 0;
			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamEnd() = 0;
	};
}
