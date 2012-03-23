/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchFirstPage.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtUserSearchFirstPage::QtUserSearchFirstPage(UserSearchWindow::Type type, const QString& title) {
	setupUi(this);
	setTitle(title);
	setSubTitle(QString(tr("%1. If you know their address you can enter it directly, or you can search for them.")).arg(type == UserSearchWindow::AddContact ? tr("Add another user to your contact list") : tr("Chat to another user")));
	connect(jid_, SIGNAL(textChanged(const QString&)), this, SLOT(emitCompletenessCheck()));
	connect(service_->lineEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(emitCompletenessCheck()));
}

bool QtUserSearchFirstPage::isComplete() const {
	bool complete = false;
	if (byJID_->isChecked()) {
		complete = JID(Q2PSTRING(jid_->text().trimmed())).isValid();
	} else if (byLocalSearch_->isChecked()) {
		complete = true;
	} else if (byRemoteSearch_->isChecked()) {
		complete = JID(Q2PSTRING(service_->currentText().trimmed())).isValid();
	}
	return complete;
}

void QtUserSearchFirstPage::emitCompletenessCheck() {
	emit completeChanged();
}

}
