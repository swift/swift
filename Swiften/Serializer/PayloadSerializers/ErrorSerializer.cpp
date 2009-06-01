#include "Swiften/Serializer/PayloadSerializers/ErrorSerializer.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

namespace Swift {

ErrorSerializer::ErrorSerializer() : GenericPayloadSerializer<Error>() {
}

String ErrorSerializer::serializePayload(boost::shared_ptr<Error> error)  const {
	String result("<error type=\"");
	switch (error->getType()) {
		case Error::Continue: result += "continue"; break;
		case Error::Modify: result += "modify"; break;
		case Error::Auth: result += "auth"; break;
		case Error::Wait: result += "wait"; break;
		default: result += "cancel"; break;
	}
	result += "\">";

	String conditionElement;
	switch (error->getCondition()) {
		case Error::BadRequest: conditionElement = "bad-request"; break;
		case Error::Conflict: conditionElement = "conflict"; break;
		case Error::FeatureNotImplemented: conditionElement = "feature-not-implemented"; break;
		case Error::Forbidden: conditionElement = "forbidden"; break;
		case Error::Gone: conditionElement = "gone"; break;
		case Error::InternalServerError: conditionElement = "internal-server-error"; break;
		case Error::ItemNotFound: conditionElement = "item-not-found"; break;
		case Error::JIDMalformed: conditionElement = "jid-malformed"; break;
		case Error::NotAcceptable: conditionElement = "not-acceptable"; break;
		case Error::NotAllowed: conditionElement = "not-allowed"; break;
		case Error::NotAuthorized: conditionElement = "not-authorized"; break;
		case Error::PaymentRequired: conditionElement = "payment-required"; break;
		case Error::RecipientUnavailable: conditionElement = "recipient-unavailable"; break;
		case Error::Redirect: conditionElement = "redirect"; break;
		case Error::RegistrationRequired: conditionElement = "registration-required"; break;
		case Error::RemoteServerNotFound: conditionElement = "remote-server-not-found"; break;
		case Error::RemoteServerTimeout: conditionElement = "remote-server-timeout"; break;
		case Error::ResourceConstraint: conditionElement = "resource-constraint"; break;
		case Error::ServiceUnavailable: conditionElement = "service-unavailable"; break;
		case Error::SubscriptionRequired: conditionElement = "subscription-required"; break;
		case Error::UnexpectedRequest: conditionElement = "unexpected-request"; break;
		default: conditionElement = "undefined-condition"; break;
	}
	result += "<" + conditionElement + " xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/>";

	if (!error->getText().isEmpty()) {
		XMLTextNode textNode(error->getText());
		result += "<text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">" + textNode.serialize() + "</text>";
	}

	result += "</error>";
	return result;
}

}
