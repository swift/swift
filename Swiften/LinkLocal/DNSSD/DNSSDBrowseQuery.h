/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>

namespace Swift {
	class DNSSDBrowseQuery {
		public:
			virtual ~DNSSDBrowseQuery();

			virtual void startBrowsing() = 0;
			virtual void stopBrowsing() = 0;

			boost::signal<void (const DNSSDServiceID&)> onServiceAdded;
			boost::signal<void (const DNSSDServiceID&)> onServiceRemoved;
			boost::signal<void ()> onError;
	};
}
