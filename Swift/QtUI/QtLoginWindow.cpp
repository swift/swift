/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtLoginWindow.h"

#include <boost/bind.hpp>
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

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestXMLConsoleUIEvent.h"
#include "Swift/Controllers/UIEvents/ToggleSoundsUIEvent.h"
#include "Swift/Controllers/UIEvents/ToggleNotificationsUIEvent.h"
#include "Swiften/Base/Platform.h"

#include "QtAboutWidget.h"
#include "QtSwiftUtil.h"
#include "QtMainWindow.h"
#include "QtUtilities.h"

namespace Swift{

QtLoginWindow::QtLoginWindow(UIEventStream* uiEventStream) : QMainWindow() {
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
	QWidget *wrapperWidget = new QWidget(this);
	wrapperWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, wrapperWidget);
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
	jidLabel->setText("<font size='-1'>User address:</font>");
	layout->addWidget(jidLabel);

	username_ = new QComboBox(this);
	username_->setEditable(true);
	username_->setWhatsThis("User address - looks like someuser@someserver.com");
	username_->setToolTip("User address - looks like someuser@someserver.com");
	layout->addWidget(username_);
	QLabel* jidHintLabel = new QLabel(this);
	jidHintLabel->setText("<font size='-1' color='grey' >Example: alice@wonderland.lit</font>");
	jidHintLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(jidHintLabel);


	QLabel* passwordLabel = new QLabel();
	passwordLabel->setText("<font size='-1'>Password:</font>");
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
	certificateButton_->setToolTip("Click if you have a personal certificate used for login to the service.");
	certificateButton_->setWhatsThis("Click if you have a personal certificate used for login to the service.");
	
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
	stack_->addWidget(wrapperWidget);
#ifdef SWIFTEN_PLATFORM_MACOSX
	menuBar_ = new QMenuBar(NULL);
#else
	menuBar_ = menuBar();
#endif
	QApplication::setQuitOnLastWindowClosed(false);
	
	swiftMenu_ = new QMenu(tr("Swift"), this);
#ifdef SWIFTEN_PLATFORM_MACOSX
	generalMenu_ = new QMenu(tr("General"), this);
#else
	generalMenu_ = swiftMenu_;
#endif
	
	QAction* aboutAction = new QAction("About Swift", this);
	connect(aboutAction, SIGNAL(triggered()), SLOT(handleAbout()));
	swiftMenu_->addAction(aboutAction);

	QAction* xmlConsoleAction = new QAction(tr("Show Debug Console"), this);
	connect(xmlConsoleAction, SIGNAL(triggered()), SLOT(handleShowXMLConsole()));
	generalMenu_->addAction(xmlConsoleAction);

	toggleSoundsAction_ = new QAction(tr("Play Sounds"), this);
	toggleSoundsAction_->setCheckable(true);
	toggleSoundsAction_->setChecked(true);
	connect(toggleSoundsAction_, SIGNAL(toggled(bool)), SLOT(handleToggleSounds(bool)));
	generalMenu_->addAction(toggleSoundsAction_);

	toggleNotificationsAction_ = new QAction(tr("Show Notifications"), this);
	toggleNotificationsAction_->setCheckable(true);
	toggleNotificationsAction_->setChecked(true);
	connect(toggleNotificationsAction_, SIGNAL(toggled(bool)), SLOT(handleToggleNotifications(bool)));
#ifdef SWIFTEN_PLATFORM_LINUX
	generalMenu_->addAction(toggleNotificationsAction_);
#endif

#ifndef SWIFTEN_PLATFORM_MACOSX
	swiftMenu_->addSeparator();
#endif
	
	QAction* quitAction = new QAction("Quit", this);
	connect(quitAction, SIGNAL(triggered()), SLOT(handleQuit()));
	swiftMenu_->addAction(quitAction);

	setInitialMenus();
	uiEventStream_->onUIEvent.connect(boost::bind(&QtLoginWindow::handleUIEvent, this, _1));
	this->show();
}

void QtLoginWindow::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<ToggleSoundsUIEvent> soundEvent = boost::dynamic_pointer_cast<ToggleSoundsUIEvent>(event);
	if (soundEvent) {
		toggleSoundsAction_->setChecked(soundEvent->getEnabled());
	}
	boost::shared_ptr<ToggleNotificationsUIEvent> notificationsEvent = boost::dynamic_pointer_cast<ToggleNotificationsUIEvent>(event);
	if (notificationsEvent) {
		toggleNotificationsAction_->setChecked(notificationsEvent->getEnabled());
	}
}

/**
 * Move and resize the window, but respect minimum sizes.
 * (Like QWidget::setGeometry, only that will truncate the window
 *  the setGeometry docs say that it shouldn't do this, but I've just seen it
 *  maybe we can remove this method if that's a Qt bug (or I'm misusing it)).
 */
void QtLoginWindow::setGentleGeometry(const QRect& rect) {
	resize(rect.size());
	move(rect.topLeft());
}

void QtLoginWindow::selectUser(const String& username) {
	for (int i = 0; i < usernames_.count(); i++) {
		if (P2QSTRING(username) == usernames_[i]) {
			username_->setCurrentIndex(i);
			password_->setFocus();
			break;
		}
	}
}

QRect QtLoginWindow::defaultPosition() {
	QDesktopWidget desktop;
	int windowWidth = 200;
	int windowHeight = 500;
	QRect screen = desktop.screenGeometry(-1); //appear on default screen
	windowWidth = std::min(windowWidth, screen.width());
	windowHeight = std::min(windowHeight, screen.height());
	int left = (screen.width() - windowWidth) / 2;
	int height = (screen.height() - windowHeight) / 2;
	return QRect(left, height, windowWidth, windowHeight);
}

void QtLoginWindow::addAvailableAccount(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) {
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
			certificateFile_ == certificateFiles_[i];
			password_->setText(passwords_[i]);
			remember_->setChecked(password_->text() != "");
		}
	}
	if (!certificateFile_.isEmpty()) {
		certificateButton_->setChecked(true);
	}
}

void QtLoginWindow::loggedOut() {
	if (stack_->count() > 1) {
		QWidget* current = stack_->currentWidget();
		stack_->setCurrentIndex(0);
		stack_->removeWidget(current);
	}
	setInitialMenus();
	setIsLoggingIn(false);
}

void QtLoginWindow::setIsLoggingIn(bool loggingIn) {
	/* Change the for loop as well if you add to this.*/
	QWidget* widgets[5] = {username_, password_, remember_, loginAutomatically_, certificateButton_};
	loginButton_->setText(loggingIn ? "Cancel" : "Connect");
	for (int i = 0; i < 5; i++) {
		widgets[i]->setEnabled(!loggingIn);
	}

}

void QtLoginWindow::loginClicked() {
	if (username_->isEnabled()) {
		onLoginRequest(Q2PSTRING(username_->currentText()), Q2PSTRING(password_->text()), Q2PSTRING(certificateFile_), remember_->isChecked(), loginAutomatically_->isChecked());
	} else {
		onCancelLoginRequest();
	}
}

void QtLoginWindow::setLoginAutomatically(bool loginAutomatically) {
	loginAutomatically_->setChecked(loginAutomatically);
}

void QtLoginWindow::handleCertficateChecked(bool checked) {
	if (checked) {
		 certificateFile_ = QFileDialog::getOpenFileName(this, "Select an authentication certificate", QString(), QString("*.cert"));
		 if (certificateFile_.isEmpty()) {
			 certificateButton_->setChecked(false);
		 }
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
	uiEventStream_->send(boost::shared_ptr<RequestXMLConsoleUIEvent>(new RequestXMLConsoleUIEvent()));
}

void QtLoginWindow::handleToggleSounds(bool enabled) {
	uiEventStream_->send(boost::shared_ptr<ToggleSoundsUIEvent>(new ToggleSoundsUIEvent(enabled)));
}

void QtLoginWindow::handleToggleNotifications(bool enabled) {
	uiEventStream_->send(boost::shared_ptr<ToggleNotificationsUIEvent>(new ToggleNotificationsUIEvent(enabled)));
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
	stack_->addWidget(qtMainWindow);
	stack_->setCurrentWidget(qtMainWindow);
	setEnabled(true);
	setInitialMenus();
	foreach (QMenu* menu, qtMainWindow->getMenus()) {
		menuBar_->addMenu(menu);
	}
}

void QtLoginWindow::setMessage(const String& message) {
	if (!message.isEmpty()) {
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
		window()->hide();
	}
}

void QtLoginWindow::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtLoginWindow::moveEvent(QMoveEvent*) {
	emit geometryChanged();	
}


}
