/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class SWIFTEN_API IQ : public Stanza {
        public:
            typedef boost::shared_ptr<IQ> ref;

            enum Type { Get, Set, Result, Error };

            IQ(Type type = Get) : type_(type) { }

            Type getType() const { return type_; }
            void setType(Type type) { type_ = type; }

            static boost::shared_ptr<IQ> createRequest(
                    Type type,
                    const JID& to,
                    const std::string& id,
                    boost::shared_ptr<Payload> payload);
            static boost::shared_ptr<IQ> createResult(
                    const JID& to,
                    const std::string& id,
                    boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>());
            static boost::shared_ptr<IQ> createResult(
                    const JID& to,
                    const JID& from,
                    const std::string& id,
                    boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>());
            static boost::shared_ptr<IQ> createError(
                    const JID& to,
                    const std::string& id,
                    ErrorPayload::Condition condition = ErrorPayload::BadRequest,
                    ErrorPayload::Type type = ErrorPayload::Cancel,
                    boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>());
            static boost::shared_ptr<IQ> createError(
                    const JID& to,
                    const JID& from,
                    const std::string& id,
                    ErrorPayload::Condition condition = ErrorPayload::BadRequest,
                    ErrorPayload::Type type = ErrorPayload::Cancel,
                    boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>());

        private:
            Type type_;
    };
}
