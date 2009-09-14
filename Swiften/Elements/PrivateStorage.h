#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Payload.h"

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
