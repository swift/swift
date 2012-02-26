/*
 * Copyright (c) 2011 Remko Tronçon
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

namespace Swift {

QtProfileWindow::QtProfileWindow(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::QtProfileWindow) {
	ui->setupUi(this);
	new QShortcut(QKeySequence::Close, this, SLOT(close()));
	ui->throbberLabel->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
	connect(ui->savePushButton, SIGNAL(clicked()), SLOT(handleSave()));
	setEditable(false);
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
}

void QtProfileWindow::setEnabled(bool b) {
	ui->vcard->setEnabled(b);
	ui->savePushButton->setEnabled(b);
}

void QtProfileWindow::setEditable(bool b) {
	if (b) {
		ui->savePushButton->show();
		ui->vcard->setEditable(true);
	} else {
		ui->savePushButton->hide();
		ui->vcard->setEditable(false);
	}
	updateTitle();
}

void QtProfileWindow::setProcessing(bool processing) {
	if (processing) {
		ui->throbberLabel->movie()->start();
		ui->throbberLabel->show();
	}
	else {
		ui->throbberLabel->hide();
		ui->throbberLabel->movie()->stop();
	}
}

void QtProfileWindow::setError(const std::string& error) {
	if (!error.empty()) {
		ui->errorLabel->setText("<font color='red'>" + Qt::escape(P2QSTRING(error)) + "</font>");
	}
	else {
		ui->errorLabel->setText("");
	}
}

void QtProfileWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
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

void QtProfileWindow::closeEvent(QCloseEvent* event) {
	onWindowClosed(jid);
	event->accept();
}

void QtProfileWindow::handleSave() {
	onVCardChangeRequest(ui->vcard->getVCard());
}

}
