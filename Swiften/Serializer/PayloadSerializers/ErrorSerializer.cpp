/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

ErrorSerializer::ErrorSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<ErrorPayload>(), serializers(serializers) {
}

std::string ErrorSerializer::serializePayload(std::shared_ptr<ErrorPayload> error)  const {
    std::string result("<error type=\"");
    switch (error->getType()) {
        case ErrorPayload::Continue: result += "continue"; break;
        case ErrorPayload::Modify: result += "modify"; break;
        case ErrorPayload::Auth: result += "auth"; break;
        case ErrorPayload::Wait: result += "wait"; break;
        case ErrorPayload::Cancel: result += "cancel"; break;
    }
    result += "\">";

    std::string conditionElement;
    switch (error->getCondition()) {
        case ErrorPayload::BadRequest: conditionElement = "bad-request"; break;
        case ErrorPayload::Conflict: conditionElement = "conflict"; break;
        case ErrorPayload::FeatureNotImplemented: conditionElement = "feature-not-implemented"; break;
        case ErrorPayload::Forbidden: conditionElement = "forbidden"; break;
        case ErrorPayload::Gone: conditionElement = "gone"; break;
        case ErrorPayload::InternalServerError: conditionElement = "internal-server-error"; break;
        case ErrorPayload::ItemNotFound: conditionElement = "item-not-found"; break;
        case ErrorPayload::JIDMalformed: conditionElement = "jid-malformed"; break;
        case ErrorPayload::NotAcceptable: conditionElement = "not-acceptable"; break;
        case ErrorPayload::NotAllowed: conditionElement = "not-allowed"; break;
        case ErrorPayload::NotAuthorized: conditionElement = "not-authorized"; break;
        case ErrorPayload::PaymentRequired: conditionElement = "payment-required"; break;
        case ErrorPayload::RecipientUnavailable: conditionElement = "recipient-unavailable"; break;
        case ErrorPayload::Redirect: conditionElement = "redirect"; break;
        case ErrorPayload::RegistrationRequired: conditionElement = "registration-required"; break;
        case ErrorPayload::RemoteServerNotFound: conditionElement = "remote-server-not-found"; break;
        case ErrorPayload::RemoteServerTimeout: conditionElement = "remote-server-timeout"; break;
        case ErrorPayload::ResourceConstraint: conditionElement = "resource-constraint"; break;
        case ErrorPayload::ServiceUnavailable: conditionElement = "service-unavailable"; break;
        case ErrorPayload::SubscriptionRequired: conditionElement = "subscription-required"; break;
        case ErrorPayload::UnexpectedRequest: conditionElement = "unexpected-request"; break;
        case ErrorPayload::UndefinedCondition: conditionElement = "undefined-condition"; break;
    }
    result += "<" + conditionElement + " xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>";

    if (!error->getText().empty()) {
        XMLTextNode textNode(error->getText());
        result += "<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">" + textNode.serialize() + "</text>";
    }

    if (error->getPayload()) {
        PayloadSerializer* serializer = serializers->getPayloadSerializer(error->getPayload());
        if (serializer) {
            result += serializer->serialize(error->getPayload());
        }
    }

    result += "</error>";
    return result;
}

}
