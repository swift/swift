/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <Swiften/Elements/FormSection.h>

namespace Swift {

FormSection::FormSection() {
}

FormSection::~FormSection() {
}

void FormSection::setLabel(const std::string& label) {
	label_ = label;
}

const std::string& FormSection::getLabel() const {
	return label_;
}

void FormSection::addTextElement(boost::shared_ptr<FormText>& textElement) {
	textElements_.push_back(textElement);
}

const std::vector<boost::shared_ptr<FormText> >& FormSection::getTextElements() const {
	return textElements_;
}

void FormSection::addReportedRef(boost::shared_ptr<FormReportedRef>& reportedRef) {
	reportedRefs_.push_back(reportedRef);
}

const std::vector<boost::shared_ptr<FormReportedRef> >& FormSection::getReportedRefs() const {
	return reportedRefs_;
}

void FormSection::addChildSection(boost::shared_ptr<FormSection>& childSection) {
	childSections_.push_back(childSection);
}

const std::vector<boost::shared_ptr<FormSection> >& FormSection::getChildSections() const {
	return childSections_;
}

void FormSection::addField(boost::shared_ptr<FormField>& field) {
	fields_.push_back(field);
}

const std::vector<boost::shared_ptr<FormField> >& FormSection::getFields() const {
	return fields_;
}

void FormSection::addFieldRef(std::string ref) {
	fieldRefs_.push_back(ref);
}

const std::vector<std::string> FormSection::getFieldRefs() const {
	return fieldRefs_;
}

}
