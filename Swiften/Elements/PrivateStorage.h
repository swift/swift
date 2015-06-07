/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SWIFTEN_API PrivateStorage : public Payload {
		public:
			PrivateStorage(boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>()) : payload(payload) {
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payload;
			}

			void setPayload(boost::shared_ptr<Payload> p) {
				payload = p;
			}

		private:
			boost::shared_ptr<Payload> payload;
	};
}
