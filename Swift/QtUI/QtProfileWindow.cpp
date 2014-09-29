/*
 * Copyright (c) 2011-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtProfileWindow.h"
#include "ui_QtProfileWindow.h"

#include <QCloseEvent>
#include <QMovie>
#include <QShortcut>
#include <QTextDocument>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtProfileWindow::QtProfileWindow() :
	QWidget(),
	ui(new Ui::QtProfileWindow) {
	ui->setupUi(this);

	ui->statusLabel->setText(tr("Retrieving profile information for this user."));
	ui->statusLabel->setVisible(false);

	ui->emptyLabel->setText(tr("No profile information is available for this user."));
	ui->emptyLabel->setVisible(false);

	new QShortcut(QKeySequence::Close, this, SLOT(close()));
	ui->throbberLabel->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
	connect(ui->savePushButton, SIGNAL(clicked()), SLOT(handleSave()));
	setEditable(false);
	setAttribute(Qt::WA_DeleteOnClose);
}

QtProfileWindow::~QtProfileWindow() {
	delete ui;
}

void QtProfileWindow::setJID(const JID& jid) {
	this->jid = jid;
	updateTitle();
}

void QtProfileWindow::setVCard(VCard::ref vcard) {
	ui->vcard->setVCard(vcard);
	if (vcard->isEmpty()) {
		ui->vcard->setVisible(false);
		ui->emptyLabel->setVisible(true);
	} else {
		ui->vcard->setVisible(true);
		ui->emptyLabel->setVisible(false);
	}

	updateWindowSize();
}

void QtProfileWindow::setEnabled(bool b) {
	ui->vcard->setEnabled(b);
	ui->savePushButton->setEnabled(b);
}

void QtProfileWindow::setEditable(bool b) {
	ui->throbberLabel->setVisible(b);
	ui->errorLabel->setVisible(b);
	ui->savePushButton->setVisible(b);
	ui->vcard->setEditable(b);
	updateTitle();
}

void QtProfileWindow::setProcessing(bool processing) {
	if (processing) {
		ui->throbberLabel->movie()->start();
		ui->throbberLabel->show();
		ui->statusLabel->setVisible(true);
		ui->vcard->setVisible(false);
	}
	else {
		ui->throbberLabel->hide();
		ui->throbberLabel->movie()->stop();
		ui->statusLabel->setVisible(false);
		ui->vcard->setVisible(true);
	}

	updateWindowSize();
}

void QtProfileWindow::setError(const std::string& error) {
	if (!error.empty()) {
		ui->errorLabel->setText("<font color='red'>" + QtUtilities::htmlEscape(P2QSTRING(error)) + "</font>");
	}
	else {
		ui->errorLabel->setText("");
	}
}

void QtProfileWindow::show() {
	QWidget::showNormal();
	QWidget::activateWindow();
	QWidget::raise();
}

void QtProfileWindow::hide() {
	QWidget::hide();
}

void QtProfileWindow::updateTitle() {
	QString jidString;
	if (jid.isValid()) {
		jidString = QString(" ( %1 )").arg(P2QSTRING(jid.toString()));
	}

	if (ui->vcard->isEditable()) {
		setWindowTitle(tr("Edit Profile") + jidString);
	} else {
		setWindowTitle(tr("Show Profile") + jidString);
	}
}

void QtProfileWindow::updateWindowSize() {
	int width = 0;
	int height = 0;

	QSize size = ui->statusLabel->size();
	width = std::max(width, size.width());
	height = std::max(height, size.height() * 3);

	size = ui->emptyLabel->size();
	width = std::max(width, size.width());
	height = std::max(height, size.height() * 3);

	size = ui->vcard->size();
	width = std::max(width, size.width());
	height = std::max(height, size.height());

	resize(width, height);
}

void QtProfileWindow::closeEvent(QCloseEvent* event) {
	event->accept();
	onWindowAboutToBeClosed(jid);
}

void QtProfileWindow::handleSave() {
	onVCardChangeRequest(ui->vcard->getVCard());
}

}
