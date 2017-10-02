/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <Swiften/Elements/FormPage.h>

namespace Swift {

FormPage::FormPage() {
}

FormPage::~FormPage() {
}

void FormPage::setLabel(const std::string& label) {
    label_ = label;
}

const std::string& FormPage::getLabel() const {
    return label_;
}

void FormPage::addChildSection(std::shared_ptr<FormSection> section) {
    childSections_.push_back(section);
}

const std::vector<std::shared_ptr<FormSection> >& FormPage::getChildSections() const {
    return childSections_;
}

void FormPage::addTextElement(std::shared_ptr<FormText> textElement) {
    textElements_.push_back(textElement);
}

const std::vector<std::shared_ptr<FormText> >& FormPage::getTextElements() const {
    return textElements_;
}

void FormPage::addReportedRef(std::shared_ptr<FormReportedRef> reportedRef) {
    reportedRefs_.push_back(reportedRef);
}

const std::vector<std::shared_ptr<FormReportedRef> >& FormPage::getReportedRefs() const {
    return reportedRefs_;
}

void FormPage::addField(std::shared_ptr<FormField> field) {
    fields_.push_back(field);
}

const std::vector<std::shared_ptr<FormField> >& FormPage::getFields() const {
    return fields_;
}

void FormPage::addFieldRef(std::string ref) {
    fieldRefs_.push_back(ref);
}

const std::vector<std::string> FormPage::getFieldRefs() const {
    return fieldRefs_;
}

}
