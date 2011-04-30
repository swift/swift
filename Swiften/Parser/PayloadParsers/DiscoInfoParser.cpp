/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/DiscoInfoParser.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

DiscoInfoParser::DiscoInfoParser() : level_(TopLevel), formParser_(NULL) {
}

void DiscoInfoParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level_ == PayloadLevel) {
		if (element == "identity") {
			getPayloadInternal()->addIdentity(DiscoInfo::Identity(attributes.getAttribute("name"), attributes.getAttribute("category"), attributes.getAttribute("type"), attributes.getAttribute("lang", "http://www.w3.org/XML/1998/namespace")));
		}
		else if (element == "feature") {
			getPayloadInternal()->addFeature(attributes.getAttribute("var"));
		}
		else if (element == "x" && ns == "jabber:x:data") {
			assert(!formParser_);
			formParser_ = new FormParser();
		}
	}
	if (formParser_) {
		formParser_->handleStartElement(element, ns, attributes);
	}
	++level_;
}

void DiscoInfoParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level_;
	if (formParser_) {
		formParser_->handleEndElement(element, ns);
	}
	if (level_ == PayloadLevel && formParser_) {
		getPayloadInternal()->addExtension(formParser_->getPayloadInternal());
		delete formParser_;
		formParser_ = NULL;
	}
}

void DiscoInfoParser::handleCharacterData(const std::string& data) {
	if (formParser_) {
		formParser_->handleCharacterData(data);
	}
}

}
