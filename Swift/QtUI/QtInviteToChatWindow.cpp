/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtInviteToChatWindow.h>

#include <QHBoxLayout>
#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtInviteToChatWindow::QtInviteToChatWindow(QWidget* parent) : QDialog(parent) {
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	//layout->setContentsMargins(0,0,0,0);
	//layout->setSpacing(2);

	QLabel* description = new QLabel(tr("Users to invite to this chat (one per line):"));
	layout->addWidget(description);
	
	jidsLayout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->addLayout(jidsLayout_);

	QLabel* reasonLabel = new QLabel(tr("If you want to provide a reason for the invitation, enter it here"));
	layout->addWidget(reasonLabel);
	reason_ = new QLineEdit(this);
	layout->addWidget(reason_);
	addJIDLine();

	connect(this, SIGNAL(accepted()), this, SLOT(handleAccepting()));
	connect(this, SIGNAL(rejected()), this, SLOT(handleRejecting()));


	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(buttonBox);

	jids_[0]->setFocus();

	setModal(false);
	show();
}

QtInviteToChatWindow::~QtInviteToChatWindow() {

}

void QtInviteToChatWindow::handleAccepting() {
	onCompleted();
}

void QtInviteToChatWindow::handleRejecting() {
	onDismissed();
}

std::string QtInviteToChatWindow::getReason() const {
	return Q2PSTRING(reason_->text());
}

std::vector<JID> QtInviteToChatWindow::getJIDs() const {
	std::vector<JID> results;
	foreach (QLineEdit* jidEdit, jids_) {
		QStringList parts = jidEdit->text().split(" ");
		if (parts.size() > 0) {
			JID jid(Q2PSTRING(parts.last()));
			if (jid.isValid() && !jid.getNode().empty()) {
				results.push_back(jid);
			}
		}
	}
	return results;
}

void QtInviteToChatWindow::addJIDLine() {
	QLineEdit* jid = new QLineEdit(this);
	QCompleter* completer = new QCompleter(&completions_, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	jid->setCompleter(completer);
	jids_.push_back(jid);
	jidsLayout_->addWidget(jid);
	connect(jid, SIGNAL(textChanged(const QString&)), this, SLOT(handleJIDTextChanged()));
}

void QtInviteToChatWindow::handleJIDTextChanged() {
	bool gotEmpty = false;
	foreach(QLineEdit* edit, jids_) {
		if (edit->text().isEmpty()) {
			gotEmpty = true;
		}
	}
	if (!gotEmpty) {
		addJIDLine();
	}
}

typedef std::pair<JID, std::string> JIDString;

void QtInviteToChatWindow::setAutoCompletions(std::vector<std::pair<JID, std::string> > completions) {
	QStringList list;
	foreach (JIDString jidPair, completions) {
		QString line = P2QSTRING(jidPair.first.toString());
		if (jidPair.second != jidPair.first.toString() && !jidPair.second.empty()) {
			line = P2QSTRING(jidPair.second) + " - " + line;
		}
		list.append(line);
	}
	completions_.setStringList(list);
}

}



