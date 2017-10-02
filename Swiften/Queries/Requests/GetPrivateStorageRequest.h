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
    class SWIFTEN_API GetPrivateStorageRequest : public Request {
        public:
            typedef std::shared_ptr<GetPrivateStorageRequest<PAYLOAD_TYPE> > ref;

            static ref create(IQRouter* router) {
                return ref(new GetPrivateStorageRequest(router));
            }

        private:
            GetPrivateStorageRequest(IQRouter* router) : Request(IQ::Get, JID(), std::make_shared<PrivateStorage>(std::make_shared<PAYLOAD_TYPE>()), router) {
            }

            virtual void handleResponse(std::shared_ptr<Payload> payload, ErrorPayload::ref error) {
                std::shared_ptr<PrivateStorage> storage = std::dynamic_pointer_cast<PrivateStorage>(payload);
                if (storage) {
                    onResponse(std::dynamic_pointer_cast<PAYLOAD_TYPE>(storage->getPayload()), error);
                }
                else {
                    onResponse(std::shared_ptr<PAYLOAD_TYPE>(), error);
                }
            }

        public:
            boost::signals2::signal<void (std::shared_ptr<PAYLOAD_TYPE>, ErrorPayload::ref)> onResponse;
    };
}
