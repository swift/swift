#include "QtLoginWindow.h"
#include "QtSwiftUtil.h"
#include "QtMainWindow.h"

#include <QBoxLayout>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <QLabel>
#include <QMenuBar>

#include <cassert>

namespace Swift{

QtLoginWindow::QtLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) : QMainWindow() {
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

	username_ = new QLineEdit(this);
	layout->addWidget(username_);

	QWidget* w = new QWidget(this);
	w->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	layout->addWidget(w);

	QHBoxLayout* credentialsLayout = new QHBoxLayout(w);
	credentialsLayout->setMargin(0);
	credentialsLayout->setSpacing(3);
	password_ = new QLineEdit(this);
	password_->setEchoMode(QLineEdit::Password);
	credentialsLayout->addWidget(password_);

	certificateButton_ = new QToolButton(this);
	certificateButton_->setCheckable(true);
	certificateButton_->setIcon(QIcon(":/icons/certificate.png"));
	certificateFile_ = P2QSTRING(defaultCertificate);
	if (!certificateFile_.isEmpty()) {
		certificateButton_->setChecked(true);
	}
	credentialsLayout->addWidget(certificateButton_);
	connect(certificateButton_, SIGNAL(clicked(bool)), SLOT(handleCertficateChecked(bool)));

	loginButton_ = new QPushButton(this);
	loginButton_->setText(tr("Connect"));
	loginButton_->setAutoDefault(true);
	loginButton_->setDefault(true);
	layout->addWidget(loginButton_);

	username_->setText(P2QSTRING(defaultJID));
	password_->setText(P2QSTRING(defaultPassword));

	message_ = new QLabel(this);
	message_->setTextFormat(Qt::RichText);
	message_->setWordWrap(true);
	layout->addWidget(message_);

	layout->addStretch();
	remember_ = new QCheckBox(tr("Remember Password?"), this);
	remember_->setChecked(defaultPassword != "");
	layout->addWidget(remember_);
	connect(loginButton_, SIGNAL(clicked()), SLOT(loginClicked()));
	stack_->addWidget(wrapperWidget);
	menuBar_ = new QMenuBar(NULL);
	setInitialMenus();
	this->show();
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
	onLoginRequest(Q2PSTRING(username_->text()), Q2PSTRING(password_->text()), Q2PSTRING(certificateFile_), remember_->isChecked());
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

void QtLoginWindow::setInitialMenus() {
	menuBar_->clear();
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

}
