/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/QtUI/QtProfileWindow.h>

#include <QCloseEvent>
#include <QMovie>
#include <QShortcut>
#include <QTextDocument>
#include <QTimer>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/ui_QtProfileWindow.h>

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
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    adjustSizeTimer.setSingleShot(true);
    connect(&adjustSizeTimer, SIGNAL(timeout()), SLOT(handleAdjustSizeTimeout()));
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
    ui->savePushButton->setVisible(b);
    ui->vcard->setEditable(b);
    updateTitle();
    updateWindowSize();
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
}

void QtProfileWindow::setError(const std::string& error) {
    if (!error.empty()) {
        ui->errorLabel->setText("<font color='red'>" + QtUtilities::htmlEscape(P2QSTRING(error)) + "</font>");
    }
    else {
        ui->errorLabel->setText("");
    }
    ui->errorLabel->setVisible(!error.empty());
}

void QtProfileWindow::show() {
    QWidget::showNormal();
    QWidget::activateWindow();
    QWidget::raise();
}

void QtProfileWindow::hide() {
    QWidget::hide();
}

QSize QtProfileWindow::sizeHint() const {
    return QWidget::sizeHint();
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
    // Delay resizing to the end of the event loop, because Qt calculates the correct layout asynchronously.
    // Qt will post LayoutRequests for widgets on the event loop on show and widgets will recaluclate their
    // layout as they process these events.
    // We use the complete and correct size hint from the freshly calculated layout by delaying execution of
    // the resize code to the end of Qt's event loop.
    if (!adjustSizeTimer.isActive()) {
        adjustSizeTimer.start(0);
    }
}

void QtProfileWindow::closeEvent(QCloseEvent* event) {
    event->accept();
    onWindowAboutToBeClosed(jid);
}

void QtProfileWindow::handleSave() {
    onVCardChangeRequest(ui->vcard->getVCard());
}

void QtProfileWindow::handleAdjustSizeTimeout() {
    // Force recaluclation of all layout geometry in children widgets.
    // This is required on Windows to have the correct size even on first show.
    QList<QWidget *> children = findChildren<QWidget*>();
    for (auto child : children) {
        child->updateGeometry();
    }

    updateGeometry();
    adjustSize();
}

}
