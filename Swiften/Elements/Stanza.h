/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/optional/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class Payload;

    class SWIFTEN_API Stanza : public ToplevelElement {
        public:
            typedef std::shared_ptr<Stanza> ref;

        protected:
            Stanza();

        public:
            virtual ~Stanza();
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Stanza)

            template<typename T>
            std::shared_ptr<T> getPayload() const {
                for (const auto& payload : payloads_) {
                    std::shared_ptr<T> result(std::dynamic_pointer_cast<T>(payload));
                    if (result) {
                        return result;
                    }
                }
                return std::shared_ptr<T>();
            }

            template<typename T>
            std::vector< std::shared_ptr<T> > getPayloads() const {
                std::vector< std::shared_ptr<T> > results;
                for (const auto& payload : payloads_) {
                    std::shared_ptr<T> result(std::dynamic_pointer_cast<T>(payload));
                    if (result) {
                        results.push_back(result);
                    }
                }
                return results;
            }


            const std::vector< std::shared_ptr<Payload> >& getPayloads() const {
                return payloads_;
            }

            void addPayload(std::shared_ptr<Payload> payload) {
                payloads_.push_back(payload);
            }

            template<typename InputIterator>
            void addPayloads(InputIterator begin, InputIterator end) {
                payloads_.insert(payloads_.end(), begin, end);
            }

            template<typename Container>
            void addPayloads(const Container& container) {
                payloads_.insert(payloads_.end(), std::begin(container), std::end(container));
            }

            void updatePayload(std::shared_ptr<Payload> payload);

            void removePayloadOfSameType(std::shared_ptr<Payload>);
            std::shared_ptr<Payload> getPayloadOfSameType(std::shared_ptr<Payload>) const;

            const JID& getFrom() const { return from_; }
            void setFrom(const JID& from) { from_ = from; }

            const JID& getTo() const { return to_; }
            void setTo(const JID& to) { to_ = to; }

            const std::string& getID() const { return id_; }
            void setID(const std::string& id) { id_ = id; }

            boost::optional<boost::posix_time::ptime> getTimestamp() const;

            // Falls back to any timestamp if no specific timestamp for the given JID is found.
            boost::optional<boost::posix_time::ptime> getTimestampFrom(const JID& jid) const;

        private:
            std::string id_;
            JID from_;
            JID to_;
            std::vector< std::shared_ptr<Payload> > payloads_;
    };
}
