/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>

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
