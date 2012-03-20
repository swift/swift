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
	class SetPrivateStorageRequest : public Request {
		public:
			typedef boost::shared_ptr<SetPrivateStorageRequest<PAYLOAD_TYPE> > ref;

			static ref create(boost::shared_ptr<PAYLOAD_TYPE> payload, IQRouter* router) {
				return ref(new SetPrivateStorageRequest<PAYLOAD_TYPE>(payload, router));
			}

		private:
			SetPrivateStorageRequest(boost::shared_ptr<PAYLOAD_TYPE> payload, IQRouter* router) : Request(IQ::Set, JID(), boost::make_shared<PrivateStorage>(payload), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload>, ErrorPayload::ref error) {
				onResponse(error);
			}

		public:
			boost::signal<void (ErrorPayload::ref)> onResponse;
	};
}
