/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/SecurityLabelParser.h>
#include <Swiften/Parser/SerializingParser.h>

namespace Swift {

SecurityLabelParser::SecurityLabelParser() : level_(TopLevel), labelParser_(0) {
}

void SecurityLabelParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	++level_;
	if (level_ == DisplayMarkingOrLabelLevel) {
		if (element == "displaymarking") {
			currentText_ = "";
			getPayloadInternal()->setBackgroundColor(attributes.getAttribute("bgcolor"));
			getPayloadInternal()->setForegroundColor(attributes.getAttribute("fgcolor"));
		}
		else if (element == "label" || element == "equivalentlabel") {
			assert(!labelParser_);
			labelParser_ = new SerializingParser();
		}
	}
	else if (level_ >= SecurityLabelLevel && labelParser_) {
		labelParser_->handleStartElement(element, ns, attributes);
	}
}

void SecurityLabelParser::handleEndElement(const std::string& element, const std::string& ns) {
	if (level_ == DisplayMarkingOrLabelLevel) {
		if (element == "displaymarking") {
			getPayloadInternal()->setDisplayMarking(currentText_);
		}
		else if (labelParser_) {
			if (element == "label") {
				getPayloadInternal()->setLabel(labelParser_->getResult());
			}
			else {
				getPayloadInternal()->addEquivalentLabel(labelParser_->getResult());
			}
			delete labelParser_;
			labelParser_ = 0;
		}
	}
	else if (labelParser_ && level_ >= SecurityLabelLevel) {
		labelParser_->handleEndElement(element, ns);
	}
	--level_;

}

void SecurityLabelParser::handleCharacterData(const std::string& data) {
	if (labelParser_) {
		labelParser_->handleCharacterData(data);
	}
	else {
		currentText_ += data;
	}
}

boost::shared_ptr<SecurityLabel> SecurityLabelParser::getLabelPayload() const {
	return getPayloadInternal();
}

}
