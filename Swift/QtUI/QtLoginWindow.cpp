/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtLoginWindow.h"

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <algorithm>
#include <cassert>

#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <QLabel>
#include <QMenuBar>
#include <QHBoxLayout>
#include <qdebug.h>
#include <QCloseEvent>
#include <QCursor>
#include <QMessageBox>
#include <QKeyEvent>
 
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestFileTransferListUIEvent.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Base/Paths.h>

#include <QtAboutWidget.h>
#include <QtSwiftUtil.h>
#include <QtMainWindow.h>
#include <QtUtilities.h>

#ifdef HAVE_SCHANNEL
#include "CAPICertificateSelector.h"
#include <Swiften/TLS/CAPICertificate.h>
#endif
#include <Swiften/TLS/PKCS12Certificate.h>

namespace Swift{

QtLoginWindow::QtLoginWindow(UIEventStream* uiEventStream, SettingsProvider* settings) : QMainWindow(), settings_(settings) {
	uiEventStream_ = uiEventStream;

	setWindowTitle("Swift");
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
	QtUtilities::setX11Resource(this, "Main");

	resize(200, 500);
	setContentsMargins(0,0,0,0);
	QWidget *centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	QBoxLayout *topLayout = new QBoxLayout(QBoxLayout::TopToBottom, centralWidget);
	stack_ = new QStackedWidget(centralWidget);
	topLayout->addWidget(stack_);
	topLayout->setMargin(0);
	loginWidgetWrapper_ = new QWidget(this);
	loginWidgetWrapper_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, loginWidgetWrapper_);
	layout->addStretch(2);

	QLabel* logo = new QLabel(this);
	logo->setPixmap(QPixmap(":/logo-shaded-text.256.png"));
	logo->setScaledContents(true);
	logo->setFixedSize(192,192);

	QWidget *logoWidget = new QWidget(this);
	QHBoxLayout *logoLayout = new QHBoxLayout();
	logoLayout->setMargin(0);
	logoLayout->addStretch(0);
	logoLayout->addWidget(logo);
	logoLayout->addStretch(0);
	logoWidget->setLayout(logoLayout);
	layout->addWidget(logoWidget);

	layout->addStretch(2);

	QLabel* jidLabel = new QLabel(this);
	jidLabel->setText("<font size='-1'>" + tr("User address:") + "</font>");
	layout->addWidget(jidLabel);

	username_ = new QComboBox(this);
	username_->setEditable(true);
	username_->setWhatsThis(tr("User address - looks like someuser@someserver.com"));
	username_->setToolTip(tr("User address - looks like someuser@someserver.com"));
	username_->view()->installEventFilter(this);
	layout->addWidget(username_);
	QLabel* jidHintLabel = new QLabel(this);
	jidHintLabel->setText("<font size='-1' color='grey' >" + tr("Example: alice@wonderland.lit") + "</font>");
	jidHintLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(jidHintLabel);


	QLabel* passwordLabel = new QLabel();
	passwordLabel->setText("<font size='-1'>" + tr("Password:") + "</font>");
	layout->addWidget(passwordLabel);


	QWidget* w = new QWidget(this);
	w->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	layout->addWidget(w);

	QHBoxLayout* credentialsLayout = new QHBoxLayout(w);
	credentialsLayout->setMargin(0);
	credentialsLayout->setSpacing(3);
	password_ = new QLineEdit(this);
	password_->setEchoMode(QLineEdit::Password);
	connect(password_, SIGNAL(returnPressed()), this, SLOT(loginClicked()));
	connect(username_->lineEdit(), SIGNAL(returnPressed()), password_, SLOT(setFocus()));
	connect(username_, SIGNAL(editTextChanged(const QString&)), this, SLOT(handleUsernameTextChanged()));
	credentialsLayout->addWidget(password_);

	certificateButton_ = new QToolButton(this);
	certificateButton_->setCheckable(true);
	certificateButton_->setIcon(QIcon(":/icons/certificate.png"));
	certificateButton_->setToolTip(tr("Click if you have a personal certificate used for login to the service."));
	certificateButton_->setWhatsThis(tr("Click if you have a personal certificate used for login to the service."));

	credentialsLayout->addWidget(certificateButton_);
	connect(certificateButton_, SIGNAL(clicked(bool)), SLOT(handleCertficateChecked(bool)));

	loginButton_ = new QPushButton(this);
	loginButton_->setText(tr("Connect"));
	loginButton_->setAutoDefault(true);
	loginButton_->setDefault(true);
	layout->addWidget(loginButton_);

	message_ = new QLabel(this);
	message_->setTextFormat(Qt::RichText);
	message_->setWordWrap(true);
	layout->addWidget(message_);

	layout->addStretch(2);
	remember_ = new QCheckBox(tr("Remember Password?"), this);
	layout->addWidget(remember_);
	loginAutomatically_ = new QCheckBox(tr("Login Automatically?"), this);
	layout->addWidget(loginAutomatically_);

	connect(loginButton_, SIGNAL(clicked()), SLOT(loginClicked()));
	stack_->addWidget(loginWidgetWrapper_);
#ifdef SWIFTEN_PLATFORM_MACOSX
	menuBar_ = new QMenuBar(NULL);
#else
	menuBar_ = menuBar();
#endif
	QApplication::setQuitOnLastWindowClosed(false);

	swiftMenu_ = new QMenu(tr("&Swift"), this);
#ifdef SWIFTEN_PLATFORM_MACOSX
	generalMenu_ = new QMenu(tr("&General"), this);
#else
	generalMenu_ = swiftMenu_;
#endif

#ifdef SWIFTEN_PLATFORM_MACOSX
	QAction* aboutAction = new QAction(QString("&About %1").arg("Swift"), this);
#else
	QAction* aboutAction = new QAction(QString(tr("&About %1")).arg("Swift"), this);
#endif
	connect(aboutAction, SIGNAL(triggered()), SLOT(handleAbout()));
	swiftMenu_->addAction(aboutAction);

	xmlConsoleAction_ = new QAction(tr("&Show Debug Console"), this);
	connect(xmlConsoleAction_, SIGNAL(triggered()), SLOT(handleShowXMLConsole()));
	generalMenu_->addAction(xmlConsoleAction_);

#ifdef SWIFT_EXPERIMENTAL_FT
	fileTransferOverviewAction_ = new QAction(tr("Show &File Transfer Overview"), this);
	connect(fileTransferOverviewAction_, SIGNAL(triggered()), SLOT(handleShowFileTransferOverview()));
	generalMenu_->addAction(fileTransferOverviewAction_);
#endif

	toggleSoundsAction_ = new QAction(tr("&Play Sounds"), this);
	toggleSoundsAction_->setCheckable(true);
	toggleSoundsAction_->setChecked(settings_->getSetting(SettingConstants::PLAY_SOUNDS));
	connect(toggleSoundsAction_, SIGNAL(toggled(bool)), SLOT(handleToggleSounds(bool)));
	generalMenu_->addAction(toggleSoundsAction_);

	toggleNotificationsAction_ = new QAction(tr("Display Pop-up &Notifications"), this);
	toggleNotificationsAction_->setCheckable(true);
	toggleNotificationsAction_->setChecked(settings_->getSetting(SettingConstants::SHOW_NOTIFICATIONS));
	connect(toggleNotificationsAction_, SIGNAL(toggled(bool)), SLOT(handleToggleNotifications(bool)));

#ifndef SWIFTEN_PLATFORM_MACOSX
	swiftMenu_->addSeparator();
#endif

#ifdef SWIFTEN_PLATFORM_MACOSX
	QAction* quitAction = new QAction("&Quit", this);
#else
	QAction* quitAction = new QAction(tr("&Quit"), this);
#endif
	connect(quitAction, SIGNAL(triggered()), SLOT(handleQuit()));
	swiftMenu_->addAction(quitAction);

	setInitialMenus();
	settings_->onSettingChanged.connect(boost::bind(&QtLoginWindow::handleSettingChanged, this, _1));

	bool eagle = settings_->getSetting(SettingConstants::FORGET_PASSWORDS);
	remember_->setEnabled(!eagle);
	loginAutomatically_->setEnabled(!eagle);
	xmlConsoleAction_->setEnabled(!eagle);
	if (eagle) {
		remember_->setChecked(false);
		loginAutomatically_->setChecked(false);
	}

	this->show();
}

void QtLoginWindow::setShowNotificationToggle(bool toggle) {
	if (toggle) {
		generalMenu_->addAction(toggleNotificationsAction_);
	}
	else {
		generalMenu_->removeAction(toggleNotificationsAction_);
	}
}

bool QtLoginWindow::eventFilter(QObject *obj, QEvent *event) {
	if (obj == username_->view() && event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
			QString jid(username_->view()->currentIndex().data().toString());
			int result = QMessageBox::question(this, tr("Remove profile"), tr("Remove the profile '%1'?").arg(jid), QMessageBox::Yes | QMessageBox::No);
			if (result == QMessageBox::Yes) {
				onPurgeSavedLoginRequest(Q2PSTRING(jid));
			}
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

void QtLoginWindow::handleSettingChanged(const std::string& settingPath) {
	if (settingPath == SettingConstants::PLAY_SOUNDS.getKey()) {
		toggleSoundsAction_->setChecked(settings_->getSetting(SettingConstants::PLAY_SOUNDS));
	}
	if (settingPath == SettingConstants::SHOW_NOTIFICATIONS.getKey()) {
		toggleNotificationsAction_->setChecked(settings_->getSetting(SettingConstants::SHOW_NOTIFICATIONS));
	}
}

void QtLoginWindow::selectUser(const std::string& username) {
	for (int i = 0; i < usernames_.count(); i++) {
		if (P2QSTRING(username) == usernames_[i]) {
			username_->setCurrentIndex(i);
			password_->setFocus();
			break;
		}
	}
}

void QtLoginWindow::removeAvailableAccount(const std::string& jid) {
	QString username = P2QSTRING(jid);
	int index = -1;
	for (int i = 0; i < usernames_.count(); i++) {
		if (username == usernames_[i]) {
			index = i;
		}
	}
	if (index >= 0) {
		usernames_.removeAt(index);
		passwords_.removeAt(index);
		certificateFiles_.removeAt(index);
		username_->removeItem(index);
	}
}

void QtLoginWindow::addAvailableAccount(const std::string& defaultJID, const std::string& defaultPassword, const std::string& defaultCertificate) {
	QString username = P2QSTRING(defaultJID);
	int index = -1;
	for (int i = 0; i < usernames_.count(); i++) {
		if (username == usernames_[i]) {
			index = i;
		}
	}
	if (index == -1) {
		usernames_.append(username);
		passwords_.append(P2QSTRING(defaultPassword));
		certificateFiles_.append(P2QSTRING(defaultCertificate));
		username_->addItem(username);
	} else {
		usernames_[index] = username;
		passwords_[index] = P2QSTRING(defaultPassword);
		certificateFiles_[index] = P2QSTRING(defaultCertificate);
	}
}

void QtLoginWindow::handleUsernameTextChanged() {
	QString username = username_->currentText();
	for (int i = 0; i < usernames_.count(); i++) {
		if (username_->currentText() == usernames_[i]) {
			certificateFile_ = certificateFiles_[i];
			password_->setText(passwords_[i]);
			remember_->setChecked(password_->text() != "");
		}
	}
	if (!certificateFile_.isEmpty()) {
		certificateButton_->setChecked(true);
	}
}

void QtLoginWindow::loggedOut() {
	stack_->removeWidget(stack_->currentWidget());
	stack_->addWidget(loginWidgetWrapper_);
	stack_->setCurrentWidget(loginWidgetWrapper_);
	setInitialMenus();
	setIsLoggingIn(false);
}

void QtLoginWindow::setIsLoggingIn(bool loggingIn) {
	/* Change the for loop as well if you add to this.*/
	QWidget* widgets[5] = {username_, password_, remember_, loginAutomatically_, certificateButton_};
	loginButton_->setText(loggingIn ? tr("Cancel") : tr("Connect"));
	for (int i = 0; i < 5; i++) {
		widgets[i]->setEnabled(!loggingIn);
	}
	bool eagle = settings_->getSetting(SettingConstants::FORGET_PASSWORDS);
	remember_->setEnabled(!eagle);
	loginAutomatically_->setEnabled(!eagle);
}

void QtLoginWindow::loginClicked() {
	if (username_->isEnabled()) {
		std::string banner = settings_->getSetting(QtUISettingConstants::CLICKTHROUGH_BANNER);
		if (!banner.empty()) {
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Confirm terms of use"));
			msgBox.setText("");
			msgBox.setInformativeText(P2QSTRING(banner));
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			if (msgBox.exec() != QMessageBox::Yes) {
				return;
			}
		}
		CertificateWithKey::ref certificate;
		std::string certificateString = Q2PSTRING(certificateFile_);
#if defined(HAVE_SCHANNEL)
		if (isCAPIURI(certificateString)) {
			certificate = boost::make_shared<CAPICertificate>(certificateString);
		} else {
			certificate = boost::make_shared<PKCS12Certificate>(certificateString, createSafeByteArray(Q2PSTRING(password_->text())));
		}
#else
		certificate = boost::make_shared<PKCS12Certificate>(certificateString, createSafeByteArray(Q2PSTRING(password_->text())));
#endif

		onLoginRequest(Q2PSTRING(username_->currentText()), Q2PSTRING(password_->text()), certificateString, certificate, remember_->isChecked(), loginAutomatically_->isChecked());
		if (settings_->getSetting(SettingConstants::FORGET_PASSWORDS)) { /* Mustn't remember logins */
			username_->clearEditText();
			password_->setText("");
		}
	} else {
		onCancelLoginRequest();
	}
}

void QtLoginWindow::setLoginAutomatically(bool loginAutomatically) {
	loginAutomatically_->setChecked(loginAutomatically);
}

void QtLoginWindow::handleCertficateChecked(bool checked) {
	if (checked) {
#ifdef HAVE_SCHANNEL
		certificateFile_ = P2QSTRING(selectCAPICertificate());
		if (certificateFile_.isEmpty()) {
			certificateButton_->setChecked(false);
		}
#else
		certificateFile_ = QFileDialog::getOpenFileName(this, tr("Select an authentication certificate"), QString(), QString("*.cert;*.p12;*.pfx"));
		if (certificateFile_.isEmpty()) {
			certificateButton_->setChecked(false);
		}
#endif
	}
	else {
		certificateFile_ = "";
	}
}

void QtLoginWindow::handleAbout() {
	if (!aboutDialog_) {
		aboutDialog_ = new QtAboutWidget();
		aboutDialog_->show();
	}
	else {
		aboutDialog_->show();
		aboutDialog_->raise();
		aboutDialog_->activateWindow();
	}
}

void QtLoginWindow::handleShowXMLConsole() {
	uiEventStream_->send(boost::make_shared<RequestXMLConsoleUIEvent>());
}

void QtLoginWindow::handleShowFileTransferOverview() {
	uiEventStream_->send(boost::make_shared<RequestFileTransferListUIEvent>());
}

void QtLoginWindow::handleToggleSounds(bool enabled) {
	settings_->storeSetting(SettingConstants::PLAY_SOUNDS, enabled);
}

void QtLoginWindow::handleToggleNotifications(bool enabled) {
	settings_->storeSetting(SettingConstants::SHOW_NOTIFICATIONS, enabled);
}

void QtLoginWindow::handleQuit() {
	onQuitRequest();
}

void QtLoginWindow::quit() {
	QApplication::quit();
}

void QtLoginWindow::setInitialMenus() {
	menuBar_->clear();
	menuBar_->addMenu(swiftMenu_);
#ifdef SWIFTEN_PLATFORM_MACOSX
	menuBar_->addMenu(generalMenu_);
#endif
}

void QtLoginWindow::morphInto(MainWindow *mainWindow) {
	QtMainWindow *qtMainWindow = dynamic_cast<QtMainWindow*>(mainWindow);
	assert(qtMainWindow);
	stack_->removeWidget(loginWidgetWrapper_);
	stack_->addWidget(qtMainWindow);
	stack_->setCurrentWidget(qtMainWindow);
	setEnabled(true);
	setInitialMenus();
	foreach (QMenu* menu, qtMainWindow->getMenus()) {
		menuBar_->addMenu(menu);
	}
}

void QtLoginWindow::setMessage(const std::string& message) {
	if (!message.empty()) {
		message_->setText("<center><font color=\"red\">" + P2QSTRING(message) + "</font></center>");
	}
	else {
		message_->setText("");
	}
}

void QtLoginWindow::bringToFront() {
	if (!isVisible()) {
		window()->showNormal();
		window()->raise();
		window()->activateWindow();
	}
	else {
#ifndef Q_WS_MAC
		// FIXME: Remove this when we can bring the window back to the front using the dock on OS X
		window()->hide();
#endif
	}
}

void QtLoginWindow::hide() {
	window()->hide();
}

QtLoginWindow::QtMenus QtLoginWindow::getMenus() const {
	return QtMenus(swiftMenu_, generalMenu_);
}

void QtLoginWindow::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtLoginWindow::moveEvent(QMoveEvent*) {
	emit geometryChanged();
}

bool QtLoginWindow::askUserToTrustCertificatePermanently(const std::string& message, Certificate::ref certificate) {
	QMessageBox dialog(this);

	dialog.setText(tr("The certificate presented by the server is not valid."));
	dialog.setInformativeText(P2QSTRING(message) + "\n\n" + tr("Would you like to permanently trust this certificate? This must only be done if you know it is correct."));

	QString detailedText = tr("Subject: %1").arg(P2QSTRING(certificate->getSubjectName())) + "\n";
	detailedText += tr("SHA-1 Fingerprint: %1").arg(P2QSTRING(certificate->getSHA1Fingerprint()));
	dialog.setDetailedText(detailedText);

	dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	dialog.setDefaultButton(QMessageBox::No);

	return dialog.exec() == QMessageBox::Yes;
}

}
