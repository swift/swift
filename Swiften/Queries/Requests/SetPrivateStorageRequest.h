/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Queries/Request.h>

namespace Swift {
    template<typename PAYLOAD_TYPE>
    class SWIFTEN_API SetPrivateStorageRequest : public Request {
        public:
            typedef std::shared_ptr<SetPrivateStorageRequest<PAYLOAD_TYPE> > ref;

            static ref create(std::shared_ptr<PAYLOAD_TYPE> payload, IQRouter* router) {
                return ref(new SetPrivateStorageRequest<PAYLOAD_TYPE>(payload, router));
            }

        private:
            SetPrivateStorageRequest(std::shared_ptr<PAYLOAD_TYPE> payload, IQRouter* router) : Request(IQ::Set, JID(), std::make_shared<PrivateStorage>(payload), router) {
            }

            virtual void handleResponse(std::shared_ptr<Payload>, ErrorPayload::ref error) {
                onResponse(error);
            }

        public:
            boost::signals2::signal<void (ErrorPayload::ref)> onResponse;
    };
}
