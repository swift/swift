#include "QtJoinMUCDialog.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtJoinMUCDialog::QtJoinMUCDialog(const QString&, const QString&, QWidget* parent) : QDialog(parent) {
	setupUi(this);
	errorLabel_->hide();
	setAttribute(Qt::WA_DeleteOnClose, true);
	connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
	connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
}

void QtJoinMUCDialog::accept() {
	if (mucJID_->displayText().isEmpty()) {
		showError("You must specify a room to join.");
		return;
	}
	if (nick_->displayText().isEmpty()) {
		showError("You must specify a nickname to join a room.");
		return;
	}
	errorLabel_->hide();
	emit onJoinCommand(JID(Q2PSTRING(mucJID_->displayText())), nick_->displayText());
	QDialog::accept();
}

void QtJoinMUCDialog::showError(const QString& error) {
	errorLabel_->setText(QString("<font color='red'>%1</font>").arg(error));
	errorLabel_->show();
}
}
