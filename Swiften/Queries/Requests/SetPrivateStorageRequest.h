#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/Request.h"
#include "Swiften/Elements/PrivateStorage.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class SetPrivateStorageRequest : public Request {
		public:
			SetPrivateStorageRequest(boost::shared_ptr<PAYLOAD_TYPE> payload, IQRouter* router) : Request(IQ::Set, JID(), boost::shared_ptr<PrivateStorage>(new PrivateStorage(payload)), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> payload, boost::optional<ErrorPayload> error) {
				onResponse(error);
			}

		public:
			boost::signal<void (const boost::optional<ErrorPayload>&)> onResponse;
	};
}
