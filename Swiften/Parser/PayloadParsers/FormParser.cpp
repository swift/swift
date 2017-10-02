/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <Swiften/Parser/PayloadParsers/FormParser.h>

#include <map>

namespace Swift {

FormParser::FormParser() : level_(TopLevel), parsingItem_(false), parsingReported_(false), parsingOption_(false), hasReportedRef_(false){
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
        else if (element == "page") {
            currentPage_ = std::make_shared<FormPage>();
            currentPage_->setLabel(attributes.getAttribute("label"));
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
            currentField_ = std::make_shared<FormField>();
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
    if (level_ > PayloadLevel) {
        if (element == "section") {
            currentSection_ = std::make_shared<FormSection>();
            currentSection_->setLabel(attributes.getAttribute("label"));
            sectionStack_.push_back(currentSection_);
            currentSections_.push_back(currentSection_);
        }
        if (element == "reportedref") {
            currentReportedRef_ = std::make_shared<FormReportedRef>();
        }
        if (element == "fieldref") {
            currentText_.clear();
            currentFieldRef_ = attributes.getAttribute("var");
            if (sectionStack_.size() > 0) {
                sectionStack_.at(sectionStack_.size()-1)->addFieldRef(currentFieldRef_);
            } else if (currentPage_) {
                currentPage_->addFieldRef(currentFieldRef_);
            }
        }
        if (element == "text") {
            currentText_.clear();
            currentTextElement_ = std::make_shared<FormText>();
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
        else if (element == "page") {
            getPayloadInternal()->addPage(currentPage_);
            currentPages_.push_back(currentPage_);
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
                if (currentPages_.size() > 0) {
                    for (const auto& page : currentPages_) {
                        for (const auto& pageRef : page->getFieldRefs()) {
                            if (pageRef == currentField_->getName()) {
                                page->addField(currentField_);
                            }
                        }
                    }
                    for (const auto& section : currentSections_) {
                        for (const auto& sectionRef : section->getFieldRefs()) {
                            if (sectionRef == currentField_->getName()) {
                                section->addField(currentField_);
                            }
                        }
                    }
                } else {
                    getPayloadInternal()->addField(currentField_);
                }
            }
            currentField_.reset();
        }
    }
    if (level_ > PayloadLevel) {
        if (element == "section") {
            if (sectionStack_.size() > 1) {
                // Add the section at the top of the stack to the level below
                sectionStack_.at(sectionStack_.size()-2)->addChildSection(sectionStack_.at(sectionStack_.size()-1));
                sectionStack_.pop_back();
            }
            else if (sectionStack_.size() == 1) {
                // Add the remaining section on the stack to its parent page
                currentPage_->addChildSection(sectionStack_.at(sectionStack_.size()-1));
                sectionStack_.pop_back();
            }
        }
        if (currentReportedRef_ && !hasReportedRef_) {
            if (sectionStack_.size() > 0) {
                sectionStack_.at(sectionStack_.size()-1)->addReportedRef(currentReportedRef_);
            } else if (currentPage_) {
                currentPage_->addReportedRef(currentReportedRef_);
            }
            hasReportedRef_ = true;
            currentReportedRef_.reset();
        }
        if (currentTextElement_) {
            if (element == "text") {
                currentTextElement_->setTextString(currentText_);
            }
            if (sectionStack_.size() > 0) {
                sectionStack_.at(sectionStack_.size()-1)->addTextElement(currentTextElement_);
            } else if (currentPage_) {
                currentPage_->addTextElement(currentTextElement_);
            }
            currentTextElement_.reset();
        }
    }
}

void FormParser::handleCharacterData(const std::string& text) {
    currentText_ += text;
}

}
