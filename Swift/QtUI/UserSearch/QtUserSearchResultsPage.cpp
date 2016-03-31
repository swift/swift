/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchResultsPage.h>

namespace Swift {

QtUserSearchResultsPage::QtUserSearchResultsPage() {
    setupUi(this);
    connect(results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
    connect(results_, SIGNAL(activated(const QModelIndex&)), this, SIGNAL(onUserTriggersContinue()));
    connect(results_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
    connect(results_, SIGNAL(entered(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
    results_->setExpandsOnDoubleClick(false);
    setNoResults(false);
}

bool QtUserSearchResultsPage::isComplete() const {
    return results_->currentIndex().isValid();
}

void QtUserSearchResultsPage::setNoResults(bool noResults) {
    if (noResults) {
        results_->setEnabled(false);
        noResults_->show();
    } else {
        results_->setEnabled(true);
        noResults_->hide();
    }
}

void QtUserSearchResultsPage::emitCompletenessCheck() {
    emit completeChanged();
}

}
