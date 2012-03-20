/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/Request.h>
#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class GetPrivateStorageRequest : public Request {
		public:
			typedef boost::shared_ptr<GetPrivateStorageRequest<PAYLOAD_TYPE> > ref;

			static ref create(IQRouter* router) {
				return ref(new GetPrivateStorageRequest(router));
			}

		private:
			GetPrivateStorageRequest(IQRouter* router) : Request(IQ::Get, JID(), boost::make_shared<PrivateStorage>(boost::shared_ptr<Payload>(new PAYLOAD_TYPE())), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> payload, ErrorPayload::ref error) {
				boost::shared_ptr<PrivateStorage> storage = boost::dynamic_pointer_cast<PrivateStorage>(payload);
				if (storage) {
					onResponse(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(storage->getPayload()), error);
				}
				else {
					onResponse(boost::shared_ptr<PAYLOAD_TYPE>(), error);
				}
			}

		public:
			boost::signal<void (boost::shared_ptr<PAYLOAD_TYPE>, ErrorPayload::ref)> onResponse;
	};
}
