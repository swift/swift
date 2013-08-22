/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/FormParser.h>

#include <Swiften/Base/foreach.h>

namespace Swift {

FormParser::FormParser() : level_(TopLevel), parsingItem_(false), parsingReported_(false), parsingOption_(false) {
}

void FormParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level_ == TopLevel) {
		std::string type = attributes.getAttribute("type");
		if (type == "form") {
			getPayloadInternal()->setType(Form::FormType);
		}
		else if (type == "submit") {
			getPayloadInternal()->setType(Form::SubmitType);
		}
		else if (type == "cancel") {
			getPayloadInternal()->setType(Form::CancelType);
		}
		else if (type == "result") {
			getPayloadInternal()->setType(Form::ResultType);
		}
	}
	else if (level_ == PayloadLevel) {
		if (element == "title") {
			currentText_.clear();
		}
		else if (element == "instructions") {
			currentText_.clear();
		}
		else if (element == "reported") {
			parsingReported_ = true;
		}
		else if (element == "item") {
			parsingItem_ = true;
		}
	}
	else if (level_ == FieldLevel && currentField_) {
		currentText_.clear();
		if (element == "option") {
			currentOptionLabel_ = attributes.getAttribute("label");
			currentOptionValue_ = "";
			parsingOption_ = true;
		}
	}
	if (level_ >= PayloadLevel) {
		if (element == "field") {
			currentField_ = boost::make_shared<FormField>();
			std::string type = attributes.getAttribute("type");
			FormField::Type fieldType = FormField::UnknownType;
			if (type == "boolean") {
				fieldType = FormField::BooleanType; 
			}
			if (type == "fixed") {
				fieldType = FormField::FixedType; 
			}
			if (type == "hidden") {
				fieldType = FormField::HiddenType; 
			}
			if (type == "list-single") {
				fieldType = FormField::ListSingleType; 
			}
			if (type == "text-multi") {
				fieldType = FormField::TextMultiType; 
			}
			if (type == "text-private") {
				fieldType = FormField::TextPrivateType; 
			}
			if (type == "text-single") {
				fieldType = FormField::TextSingleType; 
			}
			if (type == "jid-single") {
				fieldType = FormField::JIDSingleType; 
			}
			if (type == "jid-multi") {
				fieldType = FormField::JIDMultiType; 
			}
			if (type == "list-multi") {
				fieldType = FormField::ListMultiType; 
			}
			currentField_->setType(fieldType);
			currentField_->setName(attributes.getAttribute("var"));
			currentField_->setLabel(attributes.getAttribute("label"));
		}
		else if (element == "value") {
			currentText_.clear();
		}
	}
	++level_;
}

void FormParser::handleEndElement(const std::string& element, const std::string&) {
	--level_;
	if (level_ == PayloadLevel) {
		if (element == "title") {
			std::string currentTitle = getPayloadInternal()->getTitle();
			if (currentTitle.empty()) {
				getPayloadInternal()->setTitle(currentText_);
			}
			else {
				getPayloadInternal()->setTitle(currentTitle + "\n" + currentText_);
			}
		}
		else if (element == "instructions") {
			std::string currentInstructions = getPayloadInternal()->getInstructions();
			if (currentInstructions.empty()) {
				getPayloadInternal()->setInstructions(currentText_);
			}
			else {
				getPayloadInternal()->setInstructions(currentInstructions + "\n" + currentText_);
			}
		}
		else if (element == "reported") {
			parsingReported_ = false;
		}
		else if (element == "item") {
			parsingItem_ = false;
			getPayloadInternal()->addItem(currentFields_);
			currentFields_.clear();
		}
	}
	else if (currentField_) {
		if (element == "required") {
			currentField_->setRequired(true);
		}
		else if (element == "desc") {
			currentField_->setDescription(currentText_);
		}
		else if (element == "option") {
			currentField_->addOption(FormField::Option(currentOptionLabel_, currentOptionValue_));
			parsingOption_ = false;
		}
		else if (element == "value") {
			if (parsingOption_) {
				currentOptionValue_ = currentText_;
			}
			else {
				currentField_->addValue(currentText_);
			}
		}
	}
	if (level_ >= PayloadLevel && currentField_) {
		if (element == "field") {
			if (parsingReported_) {
				getPayloadInternal()->addReportedField(currentField_);
			} 
			else if (parsingItem_) {
				currentFields_.push_back(currentField_);
			} 
			else {
				getPayloadInternal()->addField(currentField_);
			}
			currentField_.reset();
		}
	}
}

void FormParser::handleCharacterData(const std::string& text) {
	currentText_ += text;
}

}
