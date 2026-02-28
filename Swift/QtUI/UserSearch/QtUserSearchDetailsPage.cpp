/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtUserSearchDetailsPage.h>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <QLabel>
#include <QVBoxLayout>

#include <Swiften/JID/JID.h>

#include <Swift/QtUI/QtContactEditWidget.h>

namespace Swift {

QtUserSearchDetailsPage::QtUserSearchDetailsPage(const std::set<std::string>& groups) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Please choose a name for the contact, and select the groups you want to add the contact to.")));
    editWidget = new QtContactEditWidget(groups, this);
    layout->addWidget(editWidget);
}

QtUserSearchDetailsPage::~QtUserSearchDetailsPage() {

}

void QtUserSearchDetailsPage::setJID(const JID& jid) {
    contactJID = jid;
}

void QtUserSearchDetailsPage::setNameSuggestions(const std::vector<std::string>& nameSuggestions) {
    editWidget->setNameSuggestions(nameSuggestions);
}

void QtUserSearchDetailsPage::setName(const std::string& name) {
    editWidget->setName(name);
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
