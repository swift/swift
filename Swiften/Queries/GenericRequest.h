/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"

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

			virtual void handleResponse(boost::shared_ptr<Payload> payload, ErrorPayload::ref error) {
				onResponse(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(payload), error);
			}

		protected:
			boost::shared_ptr<PAYLOAD_TYPE> getPayloadGeneric() const {
				return boost::dynamic_pointer_cast<PAYLOAD_TYPE>(getPayload());
			}

		public:
			boost::signal<void (boost::shared_ptr<PAYLOAD_TYPE>, ErrorPayload::ref)> onResponse;
	};
}
