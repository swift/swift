#ifndef SWIFTEN_XMPPPARSERCLIENT_H
#define SWIFTEN_XMPPPARSERCLIENT_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Element.h"

namespace Swift {
	class XMPPParserClient
	{
		public:
			virtual ~XMPPParserClient();

			virtual void handleStreamStart() = 0;
			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamEnd() = 0;
	};
}

#endif
