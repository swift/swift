/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Queries/IQHandler.h>

namespace Swift {
    class IQRouter;

    /**
     * An IQ get/set request query.
     */
    class SWIFTEN_API Request : public IQHandler, public std::enable_shared_from_this<Request> {
        public:
            std::string send();

            const JID& getReceiver() const {
                return receiver_;
            }

            /**
             * Returns the ID of this request.
             * This will only be set after send() is called.
             */
            const std::string& getID() const {
                return id_;
            }


        protected:
            /**
             * Constructs a request of a certain type to a specific receiver, and attaches the given
             * payload.
             */
            Request(
                    IQ::Type type,
                    const JID& receiver,
                    std::shared_ptr<Payload> payload,
                    IQRouter* router);

            /**
             * Constructs a request of a certain type to a specific receiver from a specific sender, and attaches the given
             * payload.
             */
            Request(
                    IQ::Type type,
                    const JID& sender,
                    const JID& receiver,
                    std::shared_ptr<Payload> payload,
                    IQRouter* router);


            /**
             * Constructs a request of a certain type to a specific receiver.
             */
            Request(
                    IQ::Type type,
                    const JID& receiver,
                    IQRouter* router);

            /**
             * Constructs a request of a certain type to a specific receiver from a specific sender.
             */
            Request(
                    IQ::Type type,
                    const JID& sender,
                    const JID& receiver,
                    IQRouter* router);


            virtual void setPayload(std::shared_ptr<Payload> payload) {
                payload_ = payload;
            }

            std::shared_ptr<Payload> getPayload() const {
                return payload_;
            }

            virtual void handleResponse(std::shared_ptr<Payload>, std::shared_ptr<ErrorPayload>) = 0;

        private:
            bool handleIQ(std::shared_ptr<IQ>);
            bool isCorrectSender(const JID& jid);

        private:
            IQRouter* router_;
            IQ::Type type_;
            JID sender_;
            JID receiver_;
            std::shared_ptr<Payload> payload_;
            std::string id_;
            bool sent_;
    };
}
