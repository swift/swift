/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Queries/Request.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

Request::Request(IQ::Type type, const JID& receiver, std::shared_ptr<Payload> payload, IQRouter* router) : router_(router), type_(type), receiver_(receiver), payload_(payload), sent_(false) {
}

Request::Request(IQ::Type type, const JID& receiver, IQRouter* router) : router_(router), type_(type), receiver_(receiver), sent_(false) {
}

Request::Request(IQ::Type type, const JID& sender, const JID& receiver, std::shared_ptr<Payload> payload, IQRouter* router) : router_(router), type_(type), sender_(sender), receiver_(receiver), payload_(payload), sent_(false) {
}

Request::Request(IQ::Type type, const JID& sender, const JID& receiver, IQRouter* router) : router_(router), type_(type), sender_(sender), receiver_(receiver), sent_(false) {
}

std::string Request::send() {
    assert(payload_);
    assert(!sent_);
    sent_ = true;

    std::shared_ptr<IQ> iq(new IQ(type_));
    iq->setTo(receiver_);
    iq->setFrom(sender_);
    iq->addPayload(payload_);
    id_ = router_->getNewIQID();
    iq->setID(id_);

    try {
        router_->addHandler(shared_from_this());
    }
    catch (const std::exception&) {
        router_->addHandler(this);
    }

    router_->sendIQ(iq);
    return id_;
}

bool Request::handleIQ(std::shared_ptr<IQ> iq) {
    bool handled = false;
    if (iq->getType() == IQ::Result || iq->getType() == IQ::Error) {
        if (sent_ && iq->getID() == id_) {
            if (isCorrectSender(iq->getFrom())) {
                if (iq->getType() == IQ::Result) {
                    std::shared_ptr<Payload> payload = iq->getPayloadOfSameType(payload_);
                    if (!payload && std::dynamic_pointer_cast<RawXMLPayload>(payload_) && !iq->getPayloads().empty()) {
                        payload = iq->getPayloads().front();
                    }
                    handleResponse(payload, ErrorPayload::ref());
                }
                else {
                    ErrorPayload::ref errorPayload = iq->getPayload<ErrorPayload>();
                    if (errorPayload) {
                        handleResponse(std::shared_ptr<Payload>(), errorPayload);
                    }
                    else {
                        handleResponse(std::shared_ptr<Payload>(), ErrorPayload::ref(new ErrorPayload(ErrorPayload::UndefinedCondition)));
                    }
                }
                router_->removeHandler(this);
                handled = true;
            }
        }
    }
    return handled;
}

bool Request::isCorrectSender(const JID& jid) {
    if (router_->isAccountJID(receiver_)) {
        if (jid.isValid() && jid.equals(router_->getJID(), JID::WithResource)) {
            // This unspecified behavior seems to happen in ejabberd versions (e.g. 2.0.5)
            SWIFT_LOG(warning) << "Server responded to an account request with a full JID, which is not allowed. Handling it anyway.";
            return true;
        }
        return router_->isAccountJID(jid);
    }
    else {
        return jid.equals(receiver_, JID::WithResource);
    }
}


}
