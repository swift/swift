/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtAddContactDialog.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtAddContactDialog::QtAddContactDialog(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	errorLabel_->hide();
	setAttribute(Qt::WA_DeleteOnClose, true);
	connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
	connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
}

void QtAddContactDialog::accept() {
	if (contactJID_->displayText().isEmpty()) {
		showError("You must specify a contact to add.");
		return;
	}
	if (name_->displayText().isEmpty()) {
		showError("You must specify a name for the contact.");
		return;
	}
	errorLabel_->hide();
	emit onAddCommand(JID(Q2PSTRING(contactJID_->displayText())), name_->displayText());
	QDialog::accept();
}

void QtAddContactDialog::showError(const QString& error) {
	errorLabel_->setText(QString("<font color='red'>%1</font>").arg(error));
	errorLabel_->show();
}
}
