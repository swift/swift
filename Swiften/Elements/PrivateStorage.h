/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class PrivateStorage : public Payload {
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
