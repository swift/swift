/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API JinglePayload : public Payload {
        public:
            typedef std::shared_ptr<JinglePayload> ref;
            struct Reason : public Payload {
                    enum Type {
                        UnknownType,
                        AlternativeSession,
                        Busy,
                        Cancel,
                        ConnectivityError,
                        Decline,
                        Expired,
                        FailedApplication,
                        FailedTransport,
                        GeneralError,
                        Gone,
                        IncompatibleParameters,
                        MediaError,
                        SecurityError,
                        Success,
                        Timeout,
                        UnsupportedApplications,
                        UnsupportedTransports
                    };
                    Reason() : type(UnknownType), text("") {}
                    Reason(Type type, const std::string& text = "") : type(type), text(text) {}
                    Type type;
                    std::string text;
            };

            enum Action {
                UnknownAction,
                ContentAccept,
                ContentAdd,
                ContentModify,
                ContentReject,
                ContentRemove,
                DescriptionInfo,
                SecurityInfo,
                SessionAccept,
                SessionInfo,
                SessionInitiate,
                SessionTerminate,
                TransportAccept,
                TransportInfo,
                TransportReject,
                TransportReplace
            };
            JinglePayload() : action(SessionTerminate), sessionID("") {
            }

            JinglePayload(Action action, const std::string& sessionID) : action(action), sessionID(sessionID) {

            }

            void setAction(Action action) {
                this->action = action;
            }

            Action getAction() const {
                return action;
            }

            void setInitiator(const JID& initiator) {
                this->initiator = initiator;
            }

            const JID& getInitiator() const {
                return initiator;
            }

            void setResponder(const JID& responder) {
                this->responder = responder;
            }

            const JID& getResponder() const {
                return responder;
            }

            void setSessionID(const std::string& id) {
                sessionID = id;
            }

            const std::string& getSessionID() const {
                return sessionID;
            }

            void addContent(JingleContentPayload::ref content) {
                this->payloads.push_back(content);
            }

            void addPayload(std::shared_ptr<Payload> payload) {
                this->payloads.push_back(payload);
            }

            const std::vector<JingleContentPayload::ref> getContents() const {
                return getPayloads<JingleContentPayload>();
            }

            const std::vector<std::shared_ptr<Payload> > getPayloads() const {
                return payloads;
            }

            template<typename T>
            const std::vector<std::shared_ptr<T> > getPayloads() const {
                std::vector<std::shared_ptr<T> > matched_payloads;
                for (const auto& payload : payloads) {
                    std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(payload);
                    if (result) {
                        matched_payloads.push_back(result);
                    }
                }

                return matched_payloads;

            }

            template<typename T>
            const std::shared_ptr<T> getPayload() const {
                std::shared_ptr<T> result;
                for (const auto& payload : payloads) {
                    result = std::dynamic_pointer_cast<T>(payload);
                    if (result) {
                        return result;
                    }
                }

                return result;
            }

            void setReason(const Reason& reason) {
                this->reason = reason;
            }

            const boost::optional<Reason>& getReason() const {
                return reason;
            }

        private:
            Action action;
            JID initiator;
            JID responder;
            std::string sessionID;
            std::vector<std::shared_ptr<Payload> > payloads;
            boost::optional<Reason> reason;
    };
}
