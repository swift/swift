/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/Form.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

std::string Form::getFormType() const {
	FormField::ref field = getField("FORM_TYPE");
	boost::shared_ptr<HiddenFormField> f = boost::dynamic_pointer_cast<HiddenFormField>(field);
	return (f ? f->getValue() : "");
}

FormField::ref Form::getField(const std::string& name) const {
	foreach(FormField::ref field, fields_) {
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

}
