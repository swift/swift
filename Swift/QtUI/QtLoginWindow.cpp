#include "QtLoginWindow.h"

#include "QtAboutWidget.h"
#include "QtSwiftUtil.h"
#include "QtMainWindow.h"

#include <algorithm>

#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <QLabel>
#include <QMenuBar>

#include <cassert>

namespace Swift{

QtLoginWindow::QtLoginWindow() : QMainWindow() {
	setWindowTitle("Swift");
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
	layout->addStretch();

	QLabel* logo = new QLabel(this);
	logo->setPixmap(QPixmap(":/logo-shaded-text.256.png"));
	logo->setScaledContents(true);
	logo->setFixedSize(192,192);
	layout->addWidget(logo);
	layout->addStretch();

	QLabel* jidLabel = new QLabel(this);
	jidLabel->setText("<font size='-1'>User address</font>");
	layout->addWidget(jidLabel);

	username_ = new QComboBox(this);
	username_->setEditable(true);
	layout->addWidget(username_);

	QLabel* passwordLabel = new QLabel();
	passwordLabel->setText("<font size='-1'>Password</font>");
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

	layout->addStretch();
	remember_ = new QCheckBox(tr("Remember Password?"), this);
	layout->addWidget(remember_);
	connect(loginButton_, SIGNAL(clicked()), SLOT(loginClicked()));
	stack_->addWidget(wrapperWidget);
#ifdef SWIFTEN_PLATFORM_MACOSX
	menuBar_ = new QMenuBar(NULL);
#else
	menuBar_ = menuBar();
#endif
	QApplication::setQuitOnLastWindowClosed(false);
	
	swiftMenu_ = new QMenu(tr("Swift"), this);
	
	QAction* aboutAction = new QAction("About Swift", this);
	connect(aboutAction, SIGNAL(activated()), SLOT(handleAbout()));
	swiftMenu_->addAction(aboutAction);
	
	QAction* quitAction = new QAction("Quit", this);
	connect(quitAction, SIGNAL(activated()), SLOT(handleQuit()));
	swiftMenu_->addAction(quitAction);
	
	setInitialMenus();
	this->show();
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
		}
	}
	if (!certificateFile_.isEmpty()) {
		certificateButton_->setChecked(true);
	}
	remember_->setChecked(password_->text() != "");
}

void QtLoginWindow::loggedOut() {
	if (stack_->count() > 1) {
		QWidget* current = stack_->currentWidget();
		stack_->setCurrentIndex(0);
		stack_->removeWidget(current);
	}
	setInitialMenus();
	setEnabled(true);
}

void QtLoginWindow::loginClicked() {
	setEnabled(false);
	message_->setText("");
	onLoginRequest(Q2PSTRING(username_->currentText()), Q2PSTRING(password_->text()), Q2PSTRING(certificateFile_), remember_->isChecked());
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

void QtLoginWindow::handleQuit() {
	QApplication::quit();
}

void QtLoginWindow::setInitialMenus() {
	menuBar_->clear();
	menuBar_->addMenu(swiftMenu_);
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
	if (isHidden()) {
		showNormal();
		raise();
		activateWindow();
	}
	else {
		hide();
	}
}

void QtLoginWindow::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtLoginWindow::moveEvent(QMoveEvent*) {
	emit geometryChanged();	
}


}
