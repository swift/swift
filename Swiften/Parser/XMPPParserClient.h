/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
	
	class ProtocolHeader;

	class SWIFTEN_API XMPPParserClient {
		public:
			virtual ~XMPPParserClient();

			virtual void handleStreamStart(const ProtocolHeader&) = 0;
			virtual void handleElement(boost::shared_ptr<ToplevelElement>) = 0;
			virtual void handleStreamEnd() = 0;
	};
}
