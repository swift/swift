#pragma once

#include <boost/signal.hpp>

#include "Swiften/Queries/Request.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class GenericRequest : public Request {
		public:
			GenericRequest(
					IQ::Type type, 
					const JID& receiver, 
					boost::shared_ptr<Payload> payload, 
					IQRouter* router) :
						Request(type, receiver, payload, router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> payload, boost::optional<ErrorPayload> error) {
				onResponse(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(payload), error);
			}

		public:
			boost::signal<void (boost::shared_ptr<PAYLOAD_TYPE>, const boost::optional<ErrorPayload>&)> onResponse;
	};
}
