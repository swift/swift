/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/ErrorParser.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>

namespace Swift {

ErrorParser::ErrorParser(PayloadParserFactoryCollection* factories) : factories(factories), level_(TopLevel) {
}

void ErrorParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level_ == TopLevel) {
		std::string type = attributes.getAttribute("type");
		if (type == "continue") {
			getPayloadInternal()->setType(ErrorPayload::Continue);
		}
		else if (type == "modify") {
			getPayloadInternal()->setType(ErrorPayload::Modify);
		}
		else if (type == "auth") {
			getPayloadInternal()->setType(ErrorPayload::Auth);
		}
		else if (type == "wait") {
			getPayloadInternal()->setType(ErrorPayload::Wait);
		}
		else {
			getPayloadInternal()->setType(ErrorPayload::Cancel);
		}
	}
	else if (level_ == PayloadLevel) {
		if (element == "text") {

		}
		else if (element == "bad-request") {
			getPayloadInternal()->setCondition(ErrorPayload::BadRequest);
		}
		else if (element == "conflict") {
			getPayloadInternal()->setCondition(ErrorPayload::Conflict);
		}
		else if (element == "feature-not-implemented") {
			getPayloadInternal()->setCondition(ErrorPayload::FeatureNotImplemented);
		}
		else if (element == "forbidden") {
			getPayloadInternal()->setCondition(ErrorPayload::Forbidden);
		}
		else if (element == "gone") {
			getPayloadInternal()->setCondition(ErrorPayload::Gone);
		}
		else if (element == "internal-server-error") {
			getPayloadInternal()->setCondition(ErrorPayload::InternalServerError);
		}
		else if (element == "item-not-found") {
			getPayloadInternal()->setCondition(ErrorPayload::ItemNotFound);
		}
		else if (element == "jid-malformed") {
			getPayloadInternal()->setCondition(ErrorPayload::JIDMalformed);
		}
		else if (element == "not-acceptable") {
			getPayloadInternal()->setCondition(ErrorPayload::NotAcceptable);
		}
		else if (element == "not-allowed") {
			getPayloadInternal()->setCondition(ErrorPayload::NotAllowed);
		}
		else if (element == "not-authorized") {
			getPayloadInternal()->setCondition(ErrorPayload::NotAuthorized);
		}
		else if (element == "payment-required") {
			getPayloadInternal()->setCondition(ErrorPayload::PaymentRequired);
		}
		else if (element == "recipient-unavailable") {
			getPayloadInternal()->setCondition(ErrorPayload::RecipientUnavailable);
		}
		else if (element == "redirect") {
			getPayloadInternal()->setCondition(ErrorPayload::Redirect);
		}
		else if (element == "registration-required") {
			getPayloadInternal()->setCondition(ErrorPayload::RegistrationRequired);
		}
		else if (element == "remote-server-not-found") {
			getPayloadInternal()->setCondition(ErrorPayload::RemoteServerNotFound);
		}
		else if (element == "remote-server-timeout") {
			getPayloadInternal()->setCondition(ErrorPayload::RemoteServerTimeout);
		}
		else if (element == "resource-constraint") {
			getPayloadInternal()->setCondition(ErrorPayload::ResourceConstraint);
		}
		else if (element == "service-unavailable") {
			getPayloadInternal()->setCondition(ErrorPayload::ServiceUnavailable);
		}
		else if (element == "subscription-required") {
			getPayloadInternal()->setCondition(ErrorPayload::SubscriptionRequired);
		}
		else if (element == "unexpected-request") {
			getPayloadInternal()->setCondition(ErrorPayload::UnexpectedRequest);
		}
		else {
			PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
			if (payloadParserFactory) {
				currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
			} else {
				getPayloadInternal()->setCondition(ErrorPayload::UndefinedCondition);
			}
		}
	}
	if (level_ >= PayloadLevel && currentPayloadParser) {
		currentPayloadParser->handleStartElement(element, ns, attributes);
	}
	++level_;
}

void ErrorParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level_;
	if (currentPayloadParser) {
		if (level_ >= PayloadLevel) {
			currentPayloadParser->handleEndElement(element, ns);
		}

		if (level_ == PayloadLevel) {
			getPayloadInternal()->setPayload(currentPayloadParser->getPayload());
			currentPayloadParser.reset();
		}
	}
	else if (level_ == PayloadLevel) {
		if (element == "text") {
			getPayloadInternal()->setText(currentText_);
		}
	}
}

void ErrorParser::handleCharacterData(const std::string& data) {
	if (level_ > PayloadLevel && currentPayloadParser) {
		currentPayloadParser->handleCharacterData(data);
	}
	else {
		currentText_ += data;
	}
}

}
