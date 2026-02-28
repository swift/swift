/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/Form.h>

namespace Swift {

std::string Form::getFormType() const {
    FormField::ref field = getField("FORM_TYPE");
    if (field && field->getType() == FormField::HiddenType) {
        return field->getValues().empty() ? "" : field->getValues()[0];
    }
    return "";
}

FormField::ref Form::getField(const std::string& name) const {
    for (const auto& field : fields_) {
        if (field->getName() == name) {
            return field;
        }
    }
    return FormField::ref();
}

void Form::addReportedField(FormField::ref field) {
    reportedFields_.push_back(field);
}

const std::vector<FormField::ref>& Form::getReportedFields() const {
    return reportedFields_;
}

void Form::addItem(const Form::FormItem& item) {
    items_.push_back(item);
}

const std::vector<Form::FormItem>& Form::getItems() const {
    return items_;
}

void Form::clearEmptyTextFields() {
    std::vector<FormField::ref> populatedFields;
    for (const auto& field : fields_) {
        if (field->getType() == FormField::TextSingleType) {
            if (!field->getTextSingleValue().empty()) {
                populatedFields.push_back(field);
            }
        }
        else if (field->getType() == FormField::TextMultiType) {
            if (!field->getTextMultiValue().empty()) {
                populatedFields.push_back(field);
            }
        }
        else {
            populatedFields.push_back(field);
        }
    }
    fields_ = populatedFields;
}

}
