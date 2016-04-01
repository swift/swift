/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchFieldsPage.h>

namespace Swift {

QtUserSearchFieldsPage::QtUserSearchFieldsPage() : formWidget_(nullptr) {
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
        formWidget_ = nullptr;
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
