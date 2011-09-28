/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <deque>
#include <boost/optional.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/logic/tribool.hpp>

#include <Swiften/Network/NATTraverser.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/NullNATTraversalInterface.h>

namespace Swift {
	class EventLoop;
	class NATTraversalGetPublicIPRequest;
	class NATTraversalForwardPortRequest;
	class NATTraversalRemovePortForwardingRequest;
	class PlatformNATTraversalRequest;
	class NATPMPInterface;
	class MiniUPnPInterface;
	class NATTraversalInterface;
	class NATPortMapping;

	class PlatformNATTraversalWorker : public NATTraverser {
			friend class PlatformNATTraversalRequest;

		public:
			PlatformNATTraversalWorker(EventLoop* eventLoop);
			~PlatformNATTraversalWorker();

			boost::shared_ptr<NATTraversalGetPublicIPRequest> createGetPublicIPRequest();
			boost::shared_ptr<NATTraversalForwardPortRequest> createForwardPortRequest(int localPort, int publicPort);
			boost::shared_ptr<NATTraversalRemovePortForwardingRequest> createRemovePortForwardingRequest(int localPort, int publicPort);

		private:
			NATTraversalInterface* getNATTraversalInterface() const;
			void addRequestToQueue(boost::shared_ptr<PlatformNATTraversalRequest>);
			void run();

		private:
			EventLoop* eventLoop;
			bool stopRequested;
			boost::thread* thread;
			std::deque<boost::shared_ptr<PlatformNATTraversalRequest> > queue;
			boost::mutex queueMutex;
			boost::condition_variable queueNonEmpty;

			NullNATTraversalInterface* nullNATTraversalInterface;
			mutable boost::logic::tribool natPMPSupported;
			mutable NATPMPInterface* natPMPInterface;
			mutable boost::logic::tribool miniUPnPSupported;
			mutable MiniUPnPInterface* miniUPnPInterface;
	};
}
