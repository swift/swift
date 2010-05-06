/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "Swiften/Roster/Roster.h"
#include "Roster/QtTreeWidget.h"
#include "SwifTools/Linkify.h"
#include "QtChatView.h"
#include "MessageSnippet.h"
#include "SystemMessageSnippet.h"
#include "QtTextEdit.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QSplitter>
#include <QString>
#include <QTextEdit>
#include <QTime>
#include <QUrl>

namespace Swift {
QtChatWindow::QtChatWindow(const QString &contact, UIEventStream* eventStream) : QtTabbable(), contact_(contact), previousMessageWasSelf_(false), previousMessageWasSystem_(false), eventStream_(eventStream) {
	unreadCount_ = 0;
	updateTitleWithUnreadCount();

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);
	

	QSplitter *logRosterSplitter = new QSplitter(this);
	layout->addWidget(logRosterSplitter);

	messageLog_ = new QtChatView(this);
	messageLog_->setFocusPolicy(Qt::NoFocus);
	logRosterSplitter->addWidget(messageLog_);

	treeWidget_ = new QtTreeWidget(eventStream_);
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

	input_ = new QtTextEdit(this);
	input_->setAcceptRichText(false);
	layout->addWidget(input_);
	
	inputClearing_ = false;
	contactIsTyping_ = false;

	connect(input_, SIGNAL(unhandledKeyPressEvent(QKeyEvent*)), messageLog_, SLOT(handleKeyPressEvent(QKeyEvent*)));
	connect(input_, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	connect(input_, SIGNAL(textChanged()), this, SLOT(handleInputChanged()));
	setFocusProxy(input_);
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(qAppFocusChanged(QWidget*, QWidget*)));

	resize(400,300);
}

QtChatWindow::~QtChatWindow() {

}

void QtChatWindow::setRosterModel(Roster* roster) {
	treeWidget_->setRosterModel(roster);	
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
	event->accept();
	emit windowClosing();
	onClosed();
}

void QtChatWindow::convertToMUC() {
	treeWidget_->show();
}

void QtChatWindow::qAppFocusChanged(QWidget *old, QWidget *now) {
	Q_UNUSED(old);
	Q_UNUSED(now);
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}
	
}

void QtChatWindow::setInputEnabled(bool enabled) {
	input_->setEnabled(enabled);
}

void QtChatWindow::showEvent(QShowEvent* event) {
	emit windowOpening();
	QWidget::showEvent(event);
}

void QtChatWindow::setUnreadMessageCount(int count) {
	unreadCount_ = count;
	updateTitleWithUnreadCount();
}

void QtChatWindow::setContactChatState(ChatState::ChatStateType state) {
	contactIsTyping_ = (state == ChatState::Composing);
	emit titleUpdated();
}

QtTabbable::AlertType QtChatWindow::getWidgetAlertState() {
	if (contactIsTyping_) {
		return ImpendingActivity;
	} 
	if (unreadCount_ > 0) {
		return WaitingActivity;
	}
	return NoActivity;
}

void QtChatWindow::setName(const String& name) {
	contact_ = P2QSTRING(name);
	updateTitleWithUnreadCount();
}

void QtChatWindow::updateTitleWithUnreadCount() {
	setWindowTitle(unreadCount_ > 0 ? QString("(%1) %2").arg(unreadCount_).arg(contact_) : contact_);
	emit titleUpdated();
}

void QtChatWindow::addMessage(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) {
	addMessage(message, senderName, senderIsSelf, label, avatarPath, "");
}

void QtChatWindow::addMessage(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath, const QString& style) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString htmlString;
	if (label) {
		htmlString = QString("<span style=\"border: thin dashed grey; padding-left: .5em; padding-right: .5em; color: %1; background-color: %2; font-size: 90%; margin-right: .5em; \">").arg(Qt::escape(P2QSTRING(label->getForegroundColor()))).arg(Qt::escape(P2QSTRING(label->getBackgroundColor())));
		htmlString += QString("%3</span> ").arg(Qt::escape(P2QSTRING(label->getDisplayMarking())));
	}
	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML.replace("\n","<br/>");
	messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
	QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
	QString styleSpanEnd = style == "" ? "" : "</span>";
	htmlString += styleSpanStart + messageHTML + styleSpanEnd;

	bool appendToPrevious = !previousMessageWasSystem_ && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_ && previousSenderName_ == P2QSTRING(senderName)));
	QString qAvatarPath =  avatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(P2QSTRING(avatarPath)).toEncoded();
	messageLog_->addMessage(MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), QDateTime::currentDateTime(), qAvatarPath, senderIsSelf, appendToPrevious));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageWasSystem_ = false;
}

void QtChatWindow::addAction(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) {
	addMessage(" *" + message + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ");
}

void QtChatWindow::addErrorMessage(const String& errorMessage) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString errorMessageHTML(Qt::escape(P2QSTRING(errorMessage)));
	errorMessageHTML.replace("\n","<br/>");
	messageLog_->addMessage(SystemMessageSnippet(QString("<span class=\"error\">%1</span>").arg(errorMessageHTML), QDateTime::currentDateTime(),previousMessageWasSystem_));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
}

void QtChatWindow::addSystemMessage(const String& message) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML.replace("\n","<br/>");
	messageLog_->addMessage(SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(),previousMessageWasSystem_));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
}

void QtChatWindow::returnPressed() {
	onSendMessageRequest(Q2PSTRING(input_->toPlainText()));
	messageLog_->scrollToBottom();
	inputClearing_ = true;
	input_->clear();
	inputClearing_ = false;
}

void QtChatWindow::handleInputChanged() {
	if (inputClearing_) {
		return;
	}
	if (input_->toPlainText().isEmpty()) {
		onUserCancelsTyping();
	} else {
		onUserTyping();
	}
}

void QtChatWindow::show() {
	QWidget::show();
	emit windowOpening();
}

void QtChatWindow::activate() {
	emit wantsToActivate();
}

}
