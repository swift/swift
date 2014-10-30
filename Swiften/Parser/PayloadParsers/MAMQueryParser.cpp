/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>
#include <Swiften/Parser/PayloadParsers/ResultSetParser.h>
#include <Swiften/Parser/PayloadParsers/MAMQueryParser.h>

using namespace Swift;

MAMQueryParser::MAMQueryParser() : level_(TopLevel) {
}

void MAMQueryParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level_ == TopLevel) {
		boost::optional<std::string> attributeValue;
		if ((attributeValue = attributes.getAttributeValue("queryid"))) {
			getPayloadInternal()->setQueryID(*attributeValue);
		}
		if ((attributeValue = attributes.getAttributeValue("node"))) {
			getPayloadInternal()->setNode(*attributeValue);
		}
	} else if (level_ == PayloadLevel) {
		if (element == "x" && ns == "jabber:x:data") {
			formParser_ = boost::make_shared<FormParser>();
		} else if (element == "set" && ns == "http://jabber.org/protocol/rsm") {
			resultSetParser_ = boost::make_shared<ResultSetParser>();
		}
	}

	if (formParser_) { /* parsing a nested Form */
		formParser_->handleStartElement(element, ns, attributes);
	}

	if (resultSetParser_) { /* parsing a nested ResultSet */
		resultSetParser_->handleStartElement(element, ns, attributes);
	}

	++level_;
}

void MAMQueryParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level_;

	if (formParser_ && level_ >= PayloadLevel) {
		formParser_->handleEndElement(element, ns);
	}
	if (formParser_ && level_ == PayloadLevel) {
		/* done parsing nested Form */
		getPayloadInternal()->setForm(boost::dynamic_pointer_cast<Form>(formParser_->getPayload()));
		formParser_.reset();
	}

	if (resultSetParser_ && level_ >= PayloadLevel) {
		resultSetParser_->handleEndElement(element, ns);
	}
	if (resultSetParser_ && level_ == PayloadLevel) {
		/* done parsing nested ResultSet */
		getPayloadInternal()->setResultSet(boost::dynamic_pointer_cast<ResultSet>(resultSetParser_->getPayload()));
		resultSetParser_.reset();
	}
}

void MAMQueryParser::handleCharacterData(const std::string& data) {
	if (formParser_) {
		formParser_->handleCharacterData(data);
	}
	if (resultSetParser_) {
		resultSetParser_->handleCharacterData(data);
	}
}
