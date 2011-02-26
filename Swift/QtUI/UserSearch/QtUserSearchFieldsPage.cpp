/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchFieldsPage.h"

namespace Swift {

QtUserSearchFieldsPage::QtUserSearchFieldsPage() {
	setupUi(this);
}

bool QtUserSearchFieldsPage::isComplete() const {
	return nickInput_->isEnabled() || firstInput_->isEnabled() || lastInput_->isEnabled() || emailInput_->isEnabled();
}

void QtUserSearchFieldsPage::emitCompletenessCheck() {
	emit completeChanged();
}

}
