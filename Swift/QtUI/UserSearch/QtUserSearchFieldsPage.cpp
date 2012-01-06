/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchFieldsPage.h"

namespace Swift {

QtUserSearchFieldsPage::QtUserSearchFieldsPage() : formWidget_(0) {
	setupUi(this);
}

bool QtUserSearchFieldsPage::isComplete() const {
	if (formWidget_) {
		return formWidget_->isEnabled();
	} else {
		return nickInput_->isEnabled() || firstInput_->isEnabled() || lastInput_->isEnabled() || emailInput_->isEnabled();
	}
}

QtFormWidget* QtUserSearchFieldsPage::getFormWidget() {
	return formWidget_;
}

void QtUserSearchFieldsPage::setFormWidget(QtFormWidget *widget) {
	if (formWidget_) {
		delete formWidget_;
		formWidget_ = NULL;
	}
	if (widget) {
		formContainer_->layout()->addWidget(widget);
	}
	formWidget_ = widget;
}

void QtUserSearchFieldsPage::emitCompletenessCheck() {
	emit completeChanged();
}

}
