/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    /**
     * reference from XEP-0372
     */
    class SWIFTEN_API ReferencePayload : public Payload {

    public:

        typedef std::shared_ptr<ReferencePayload> ref;

        enum class Type {
            Data,
            Mention,
            PubSub,
            Unknown
        };

        ReferencePayload();

        const Type& getType() const;
        const boost::optional<std::string>& getUri() const;
        const boost::optional<std::string>& getBegin() const;
        const boost::optional<std::string>& getEnd() const;
        const boost::optional<std::string>& getAnchor() const;

        const std::vector<std::shared_ptr<Payload>>& getPayloads() const;

        void setType(const Type& type);
        void setUri(const boost::optional<std::string>& uri);
        void setBegin(const boost::optional<std::string>& begin);
        void setEnd(const boost::optional<std::string>& end);
        void setAnchor(const boost::optional<std::string>& anchor);

        void addPayload(const std::shared_ptr<Payload>& payload);

    private:

        Type type_;
        boost::optional<std::string> uri_;
        boost::optional<std::string> begin_;
        boost::optional<std::string> end_;
        boost::optional<std::string> anchor_;

        std::vector<std::shared_ptr<Payload>> payloads_;
    };
}
