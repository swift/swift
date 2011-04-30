/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDBrowseQuery : public DNSSDBrowseQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDBrowseQuery(boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier) {
			}

			void startBrowsing() {
				run();
			}

			void stopBrowsing() {
				finish();
			}
	};
}
