/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/optional.hpp>

#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class DNSSDRegisterQuery {
		public:
			virtual ~DNSSDRegisterQuery();

			virtual void registerService() = 0;
			virtual void unregisterService() = 0;
			virtual void updateServiceInfo(const ByteArray& info) = 0;

			boost::signal<void (boost::optional<DNSSDServiceID>)> onRegisterFinished;
	};
}
