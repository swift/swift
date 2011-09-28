/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Network/NATTraverser.h>

namespace Swift {
	class EventLoop;

	class NullNATTraverser : public NATTraverser {
		public:
			NullNATTraverser(EventLoop* eventLoop);

			boost::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest();
			boost::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort);
			boost::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort);

		private:
			EventLoop* eventLoop;
	};
}
