/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <typeinfo>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Queries/Request.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

namespace Swift {
    class SWIFTEN_API RawRequest : public Request {
        public:
            typedef std::shared_ptr<RawRequest> ref;

            static ref create(IQ::Type type, const JID& recipient, const std::string& data, IQRouter* router) {
                return ref(new RawRequest(type, recipient, data, router));
            }

            boost::signals2::signal<void (const std::string&)> onResponse;

        private:
            RawRequest(IQ::Type type, const JID& receiver, const std::string& data, IQRouter* router) : Request(type, receiver, std::make_shared<RawXMLPayload>(data), router) {
            }

            virtual void handleResponse(std::shared_ptr<Payload> payload, ErrorPayload::ref error) {
                if (error) {
                    onResponse(ErrorSerializer(&serializers).serializePayload(error));
                }
                else {
                    assert(payload);
                    PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
                    assert(serializer);
                    onResponse(serializer->serialize(payload));
                }
            }

        private:
            FullPayloadSerializerCollection serializers;
    };
}
