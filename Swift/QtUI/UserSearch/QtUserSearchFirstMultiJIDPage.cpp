/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchFirstMultiJIDPage.h"

#include "Swift/QtUI/QtSwiftUtil.h"
#include <Swift/QtUI/UserSearch/QtContactListWidget.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/QtUI/UserSearch/QtSuggestingJIDInput.h>

namespace Swift {

QtUserSearchFirstMultiJIDPage::QtUserSearchFirstMultiJIDPage(UserSearchWindow::Type type, const QString& title, SettingsProvider* settings) {
	setupUi(this);
	setTitle(title);
	QString introText = "";
	switch (type) {
		case UserSearchWindow::AddContact:
			introText = tr("Add another user to your contact list");
			break;
		case UserSearchWindow::ChatToContact:
			introText = tr("Chat to another user");
			break;
		case UserSearchWindow::InviteToChat:
			introText = tr("Invite contact to chat");
			break;
	}

	setSubTitle(QString(tr("%1. If you know their address you can enter it directly, or you can search for them.")).arg(introText));

	contactList_ = new QtContactListWidget(this, settings);
	horizontalLayout_5->addWidget(contactList_);

	jid_ = new QtSuggestingJIDInput(this, settings);
	horizontalLayout_6->insertWidget(0, jid_);

	connect(contactList_, SIGNAL(onListChanged(std::vector<Contact>)), this, SLOT(emitCompletenessCheck()));
	connect(jid_, SIGNAL(editingDone()), this, SLOT(handleEditingDone()));
}

bool QtUserSearchFirstMultiJIDPage::isComplete() const {
	return !contactList_->getList().empty();
}

void QtUserSearchFirstMultiJIDPage::emitCompletenessCheck() {
	emit completeChanged();
}

void QtUserSearchFirstMultiJIDPage::handleEditingDone() {
	addContactButton_->setFocus();
}

}
