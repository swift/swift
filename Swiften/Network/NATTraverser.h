/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class NATTraversalGetPublicIPRequest;
	class NATTraversalForwardPortRequest;
	class NATTraversalRemovePortForwardingRequest;

	class NATTraverser {
		public:
			virtual ~NATTraverser();

			virtual boost::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest() = 0;
			virtual boost::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort) = 0;
			virtual boost::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort) = 0;
	};
}
