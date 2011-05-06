/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class EventLoop;

	class AvahiQuerier : 
			public DNSSDQuerier, 
			public boost::enable_shared_from_this<AvahiQuerier> {
		public:
			AvahiQuerier(EventLoop* eventLoop);
			~AvahiQuerier();

			boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
			boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const std::string& name, int port, const ByteArray& info);
			boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&);
			boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const std::string& hostname, int interfaceIndex);

			void start();
			void stop();
			
			AvahiThreadedPoll* getThreadedPoll() const {
				return threadedPoll;
			}

			AvahiClient* getClient() const {
				return client;
			}

		private:
			EventLoop* eventLoop;
			AvahiClient* client;
			AvahiThreadedPoll* threadedPoll;
	};
}
