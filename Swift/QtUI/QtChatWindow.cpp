#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "QtTreeWidget.h"
#include "QtTreeWidgetFactory.h"
#include "QtChatView.h"
#include "MessageSnippet.h"
#include "SystemMessageSnippet.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QSplitter>
#include <QString>
#include <QTextEdit>
#include <QTime>

namespace Swift {
QtChatWindow::QtChatWindow(const QString &contact, QtTreeWidgetFactory *treeWidgetFactory) : QtTabbable(), contact_(contact), previousMessageWasSelf_(false), previousMessageWasSystem_(false) {
	unreadCount_ = 0;
	updateTitleWithUnreadCount();

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);
	

	QSplitter *logRosterSplitter = new QSplitter(this);
	layout->addWidget(logRosterSplitter);

	messageLog_ = new QtChatView(this);
	logRosterSplitter->addWidget(messageLog_);

	treeWidget_ = dynamic_cast<QtTreeWidget*>(treeWidgetFactory->createTreeWidget());
	treeWidget_->hide();
	logRosterSplitter->addWidget(treeWidget_);


	QWidget* midBar = new QWidget(this);
	layout->addWidget(midBar);
	QHBoxLayout *midBarLayout = new QHBoxLayout(midBar);
	midBarLayout->setContentsMargins(0,0,0,0);
	midBarLayout->setSpacing(2);
	midBarLayout->addStretch();
	labelsWidget_ = new QComboBox(this);
	labelsWidget_->setFocusPolicy(Qt::NoFocus);
	labelsWidget_->hide();
	labelsWidget_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	midBarLayout->addWidget(labelsWidget_,0);

	input_ = new QLineEdit(this);
	layout->addWidget(input_);

	connect(input_, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	setFocusProxy(input_);
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(qAppFocusChanged(QWidget*, QWidget*)));

	resize(400,300);
}

TreeWidget* QtChatWindow::getTreeWidget() {
	return treeWidget_;
}

void QtChatWindow::setAvailableSecurityLabels(const std::vector<SecurityLabel>& labels) {
	availableLabels_ = labels;
	labelsWidget_->clear();
	int i = 0;
	foreach (SecurityLabel label, labels) {
		QString labelName = P2QSTRING(label.getDisplayMarking());
		labelsWidget_->addItem(labelName, QVariant(i));
		i++;
	}
}


void QtChatWindow::setSecurityLabelsError() {
	labelsWidget_->setEnabled(false);
}

void QtChatWindow::setSecurityLabelsEnabled(bool enabled) {
	if (enabled) {
		labelsWidget_->setEnabled(true);
		labelsWidget_->show();
	} else {
		labelsWidget_->hide();
	}
}

SecurityLabel QtChatWindow::getSelectedSecurityLabel() {
	assert(labelsWidget_->isEnabled());
	return availableLabels_[labelsWidget_->currentIndex()];
}

void QtChatWindow::closeEvent(QCloseEvent* event) {
	onClosed();
	emit windowClosing();
	event->accept();
}

void QtChatWindow::convertToMUC() {
	treeWidget_->show();
}

void QtChatWindow::qAppFocusChanged(QWidget *old, QWidget *now) {
	Q_UNUSED(old);
	if (now == this || now == messageLog_ || now == input_) {
		onAllMessagesRead();
	}
	
}

void QtChatWindow::showEvent(QShowEvent* event) {
	emit windowOpening();
	QWidget::showEvent(event);
}

void QtChatWindow::setUnreadMessageCount(int count) {
	unreadCount_ = count;
	updateTitleWithUnreadCount();
}

void QtChatWindow::setName(const String& name) {
	contact_ = P2QSTRING(name);
	updateTitleWithUnreadCount();
}

void QtChatWindow::updateTitleWithUnreadCount() {
	setWindowTitle(unreadCount_ > 0 ? QString("(%1) %2)").arg(unreadCount_).arg(contact_) : contact_);
	emit titleUpdated();
}

void QtChatWindow::addMessage(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) {
	if (isActiveWindow()) {
		onAllMessagesRead();
	}

	QString htmlString;
	if (label) {
		htmlString = QString("<span style=\"border: thin dashed grey; padding-left: .5em; padding-right: .5em; color: %1; background-color: %2; font-size: 90%; margin-right: .5em; \">").arg(Qt::escape(P2QSTRING(label->getForegroundColor()))).arg(Qt::escape(P2QSTRING(label->getBackgroundColor())));
		htmlString += QString("%3</span> ").arg(Qt::escape(P2QSTRING(label->getDisplayMarking())));
	}
	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML.replace("\n","<br/>");
	htmlString += messageHTML;

	bool appendToPrevious = !previousMessageWasSystem_ && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_ && previousSenderName_ == P2QSTRING(senderName)));
	messageLog_->addMessage(MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), QDateTime::currentDateTime(), (avatarPath.isEmpty() ? "qrc:/icons/avatar.png" : "file://" + P2QSTRING(avatarPath)), senderIsSelf, appendToPrevious));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageWasSystem_ = false;
}

void QtChatWindow::addErrorMessage(const String& errorMessage) {
	if (isActiveWindow()) {
		onAllMessagesRead();
	}

	QString errorMessageHTML(Qt::escape(P2QSTRING(errorMessage)));
	errorMessageHTML.replace("\n","<br/>");
	messageLog_->addMessage(SystemMessageSnippet(QString("<span class=\"error\">%1</span>").arg(errorMessageHTML), QDateTime::currentDateTime(),previousMessageWasSystem_));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
}

void QtChatWindow::addSystemMessage(const String& message) {
	if (isActiveWindow()) {
		onAllMessagesRead();
	}

	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML.replace("\n","<br/>");
	messageLog_->addMessage(SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(),previousMessageWasSystem_));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
}

void QtChatWindow::returnPressed() {
	onSendMessageRequest(Q2PSTRING(input_->text()));
	messageLog_->scrollToBottom();
	input_->clear();
}

void QtChatWindow::show() {
	QWidget::show();
	emit windowOpening();
}

}
