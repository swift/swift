/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/FormParser.h>

#include <Swiften/Base/foreach.h>

namespace Swift {

FormParser::FormParser() : level_(TopLevel), parsingItem_(false), parsingReported_(false) {
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
		else if (element == "field") {
			std::string type;
			FormField::ref correspondingReportedField;
			if (!parsingItem_) {
				type = attributes.getAttribute("type");
			} else {
				foreach(FormField::ref field, getPayloadInternal()->getReportedFields()) {
					if (field->getName() == attributes.getAttribute("var")) {
						correspondingReportedField = field;
						break;
					}
				}
			}
			if (type == "boolean" || boost::dynamic_pointer_cast<BooleanFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = BooleanFormFieldParseHelper::create();
			}
			else if (type == "fixed" || boost::dynamic_pointer_cast<FixedFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = FixedFormFieldParseHelper::create();
			}
			else if (type == "hidden" || boost::dynamic_pointer_cast<HiddenFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = HiddenFormFieldParseHelper::create();
			}
			else if (type == "jid-multi" || boost::dynamic_pointer_cast<JIDMultiFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = JIDMultiFormFieldParseHelper::create();
			}
			else if (type == "jid-single" || boost::dynamic_pointer_cast<JIDSingleFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = JIDSingleFormFieldParseHelper::create();
			}
			else if (type == "list-multi" || boost::dynamic_pointer_cast<ListMultiFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = ListMultiFormFieldParseHelper::create();
			}
			else if (type == "list-single" || boost::dynamic_pointer_cast<ListSingleFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = ListSingleFormFieldParseHelper::create();
			}
			else if (type == "text-multi" || boost::dynamic_pointer_cast<TextMultiFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = TextMultiFormFieldParseHelper::create();
			}
			else if (type == "text-private" || boost::dynamic_pointer_cast<TextPrivateFormField>(correspondingReportedField)) {
				currentFieldParseHelper_ = TextPrivateFormFieldParseHelper::create();
			}
			else /*if (type == "text-single") || undefined */ {
				currentFieldParseHelper_ = TextSingleFormFieldParseHelper::create();
			}
			if (currentFieldParseHelper_) {
				currentFieldParseHelper_->getField()->setName(attributes.getAttribute("var"));
				currentFieldParseHelper_->getField()->setLabel(attributes.getAttribute("label"));
			}
		}
		else if (element == "reported") {
			parsingReported_ = true;
			level_ = PayloadLevel - 1;
		}
		else if (element == "item") {
			parsingItem_ = true;
			level_ = PayloadLevel - 1;
		}
	}
	else if (level_ == FieldLevel && currentFieldParseHelper_) {
		currentText_.clear();
		if (element == "option") {
			currentOptionLabel_ = attributes.getAttribute("label");
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
		else if (element == "field") {
			if (currentFieldParseHelper_) {
				if (parsingReported_) {
					getPayloadInternal()->addReportedField(currentFieldParseHelper_->getField());
				} else if (parsingItem_) {
					currentFields_.push_back(currentFieldParseHelper_->getField());
				} else {
					getPayloadInternal()->addField(currentFieldParseHelper_->getField());
				}
				currentFieldParseHelper_.reset();
			}
		}
	}
	else if (level_ == FieldLevel && currentFieldParseHelper_) {
		if (element == "required") {
			currentFieldParseHelper_->getField()->setRequired(true);
		}
		else if (element == "desc") {
			currentFieldParseHelper_->getField()->setDescription(currentText_);
		}
		else if (element == "option") {
			currentFieldParseHelper_->getField()->addOption(FormField::Option(currentOptionLabel_, currentText_));
		}
		else if (element == "value") {
			currentFieldParseHelper_->addValue(currentText_);
		}
	}
	if (element == "reported") {
		parsingReported_ = false;
		level_++;
	}
	else if (element == "item") {
		parsingItem_ = false;
		level_++;
		getPayloadInternal()->addItem(currentFields_);
		currentFields_.clear();
	}
}

void FormParser::handleCharacterData(const std::string& text) {
	currentText_ += text;
}

}
