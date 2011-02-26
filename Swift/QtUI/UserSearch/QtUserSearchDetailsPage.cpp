/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchDetailsPage.h"

#include <QVBoxLayout>

#include <Swift/QtUI/QtContactEditWidget.h>

namespace Swift {

QtUserSearchDetailsPage::QtUserSearchDetailsPage(const std::set<std::string>& groups) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(tr("Please choose a name for the contact, and select the groups you want to add the contact to.")));
	editWidget = new QtContactEditWidget(groups, this);
	layout->addWidget(editWidget);
}

std::set<std::string> QtUserSearchDetailsPage::getSelectedGroups() {
	return editWidget->getSelectedGroups();
}

std::string QtUserSearchDetailsPage::getName() {
	return editWidget->getName();
}

void QtUserSearchDetailsPage::clear() {
	editWidget->clear();
}


}
