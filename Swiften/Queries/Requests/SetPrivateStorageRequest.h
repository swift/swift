/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Queries/Request.h>

namespace Swift {
    template<typename PAYLOAD_TYPE>
    class SWIFTEN_API SetPrivateStorageRequest : public Request {
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
