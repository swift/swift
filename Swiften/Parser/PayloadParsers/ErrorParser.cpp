#include "Swiften/Parser/PayloadParsers/ErrorParser.h"

namespace Swift {

ErrorParser::ErrorParser() : level_(TopLevel) {
}

void ErrorParser::handleStartElement(const String&, const String&, const AttributeMap& attributes) {
	if (level_ == TopLevel) {
		String type = attributes.getAttribute("type");
		if (type == "continue") {
			getPayloadInternal()->setType(Error::Continue);
		}
		else if (type == "modify") {
			getPayloadInternal()->setType(Error::Modify);
		}
		else if (type == "auth") {
			getPayloadInternal()->setType(Error::Auth);
		}
		else if (type == "wait") {
			getPayloadInternal()->setType(Error::Wait);
		}
		else {
			getPayloadInternal()->setType(Error::Cancel);
		}
	}
	++level_;
}

void ErrorParser::handleEndElement(const String& element, const String&) {
	--level_;
	if (level_ == PayloadLevel) {
		if (element == "text") {
			getPayloadInternal()->setText(currentText_);
		}
		else if (element == "bad-request") {
			getPayloadInternal()->setCondition(Error::BadRequest);
		}
		else if (element == "conflict") {
			getPayloadInternal()->setCondition(Error::Conflict);
		}
		else if (element == "feature-not-implemented") {
			getPayloadInternal()->setCondition(Error::FeatureNotImplemented);
		}
		else if (element == "forbidden") {
			getPayloadInternal()->setCondition(Error::Forbidden);
		}
		else if (element == "gone") {
			getPayloadInternal()->setCondition(Error::Gone);
		}
		else if (element == "internal-server-error") {
			getPayloadInternal()->setCondition(Error::InternalServerError);
		}
		else if (element == "item-not-found") {
			getPayloadInternal()->setCondition(Error::ItemNotFound);
		}
		else if (element == "jid-malformed") {
			getPayloadInternal()->setCondition(Error::JIDMalformed);
		}
		else if (element == "not-acceptable") {
			getPayloadInternal()->setCondition(Error::NotAcceptable);
		}
		else if (element == "not-allowed") {
			getPayloadInternal()->setCondition(Error::NotAllowed);
		}
		else if (element == "not-authorized") {
			getPayloadInternal()->setCondition(Error::NotAuthorized);
		}
		else if (element == "payment-required") {
			getPayloadInternal()->setCondition(Error::PaymentRequired);
		}
		else if (element == "recipient-unavailable") {
			getPayloadInternal()->setCondition(Error::RecipientUnavailable);
		}
		else if (element == "redirect") {
			getPayloadInternal()->setCondition(Error::Redirect);
		}
		else if (element == "registration-required") {
			getPayloadInternal()->setCondition(Error::RegistrationRequired);
		}
		else if (element == "remote-server-not-found") {
			getPayloadInternal()->setCondition(Error::RemoteServerNotFound);
		}
		else if (element == "remote-server-timeout") {
			getPayloadInternal()->setCondition(Error::RemoteServerTimeout);
		}
		else if (element == "resource-constraint") {
			getPayloadInternal()->setCondition(Error::ResourceConstraint);
		}
		else if (element == "service-unavailable") {
			getPayloadInternal()->setCondition(Error::ServiceUnavailable);
		}
		else if (element == "subscription-required") {
			getPayloadInternal()->setCondition(Error::SubscriptionRequired);
		}
		else if (element == "unexpected-request") {
			getPayloadInternal()->setCondition(Error::UnexpectedRequest);
		}
		else {
			getPayloadInternal()->setCondition(Error::UndefinedCondition);
		}
	}
}

void ErrorParser::handleCharacterData(const String& data) {
	currentText_ += data;
}

}
