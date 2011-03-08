/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Roster/QtTreeWidget.h"
#include "SwifTools/Linkify.h"
#include "QtChatView.h"
#include "MessageSnippet.h"
#include "SystemMessageSnippet.h"
#include "QtTextEdit.h"
#include "QtScaledAvatarCache.h"

#include "SwifTools/TabComplete.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QComboBox>
#include <QFileInfo>
#include <QLineEdit>
#include <QSplitter>
#include <QString>
#include <QTextEdit>
#include <QTime>
#include <QUrl>

namespace Swift {
QtChatWindow::QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream) : QtTabbable(), contact_(contact), previousMessageWasSelf_(false), previousMessageWasSystem_(false), previousMessageWasPresence_(false), eventStream_(eventStream) {
	unreadCount_ = 0;
	inputEnabled_ = true;
	completer_ = NULL;
	theme_ = theme;
	updateTitleWithUnreadCount();

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);
	

	QSplitter *logRosterSplitter = new QSplitter(this);
	logRosterSplitter->setAutoFillBackground(true);
	layout->addWidget(logRosterSplitter);

	messageLog_ = new QtChatView(theme, this);
	logRosterSplitter->addWidget(messageLog_);

	treeWidget_ = new QtTreeWidget(eventStream_);
	treeWidget_->setEditable(false);
	treeWidget_->hide();
	logRosterSplitter->addWidget(treeWidget_);
	logRosterSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget* midBar = new QWidget(this);
	layout->addWidget(midBar);
	midBar->setAutoFillBackground(true);
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

	connect(input_, SIGNAL(unhandledKeyPressEvent(QKeyEvent*)), this, SLOT(handleKeyPressEvent(QKeyEvent*)));
	connect(input_, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	connect(input_, SIGNAL(textChanged()), this, SLOT(handleInputChanged()));
	setFocusProxy(input_);
	logRosterSplitter->setFocusProxy(input_);
	midBar->setFocusProxy(input_);
	messageLog_->setFocusProxy(input_);
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(qAppFocusChanged(QWidget*, QWidget*)));
	connect(messageLog_, SIGNAL(gotFocus()), input_, SLOT(setFocus()));
	resize(400,300);
}

QtChatWindow::~QtChatWindow() {

}

void QtChatWindow::setTabComplete(TabComplete* completer) {
	completer_ = completer;
}

void QtChatWindow::handleKeyPressEvent(QKeyEvent* event) {
	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();
	if (key == Qt::Key_W && modifiers == Qt::ControlModifier) {
		close();
	} else if (
		(key == Qt::Key_PageUp && modifiers == Qt::ControlModifier)
//		|| (key == Qt::Key_Left && modifiers == (Qt::ControlModifier & Qt::ShiftModifier))
	) {
		emit requestPreviousTab();
	} else if (
		(key == Qt::Key_PageDown && modifiers == Qt::ControlModifier)
//		|| (key == Qt::Key_Right && modifiers == (Qt::ControlModifier & Qt::ShiftModifier)
		|| (key == Qt::Key_Tab && modifiers == Qt::ControlModifier)
	) {
		emit requestNextTab();
	} else if (
		(key == Qt::Key_A && modifiers == Qt::AltModifier)
	)  {
		emit requestActiveTab();
	} else if (key == Qt::Key_Tab) {
		tabComplete();
	} else {
		messageLog_->handleKeyPressEvent(event);
	}
}

void QtChatWindow::tabComplete() {
	if (!completer_) {
		return;
	}
//	QTextDocument* document = input_->document();
	QTextCursor cursor = input_->textCursor();
	cursor.select(QTextCursor::WordUnderCursor);
	QString root = cursor.selectedText();
	bool firstWord = cursor.selectionStart() == 0;
	if (root.isEmpty()) {
		return;
	}
	QString suggestion = P2QSTRING(completer_->completeWord(Q2PSTRING(root)));
	if (root == suggestion) {
		return;
	}
	cursor.beginEditBlock();
	cursor.removeSelectedText();
	cursor.insertText(suggestion);
	if (firstWord) {
		//	cursor.insertText(":");
	}
	//cursor.insertText(" ");
	cursor.endEditBlock();
}

void QtChatWindow::setRosterModel(Roster* roster) {
	treeWidget_->setRosterModel(roster);	
}

void QtChatWindow::setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {
	availableLabels_ = labels;
	labelsWidget_->clear();
	int i = 0;
	int defaultIndex = 0;
	foreach (SecurityLabelsCatalog::Item label, labels) {
		std::string selector = label.getSelector();
		std::string displayMarking = label.getLabel() ? label.getLabel()->getDisplayMarking() : "";
		QString labelName = selector.empty() ? displayMarking.c_str() : selector.c_str();
		labelsWidget_->addItem(labelName, QVariant(i));
		if (label.getIsDefault()) {
			defaultIndex = i;
		}
		i++;
	}
	labelsWidget_->setCurrentIndex(defaultIndex);
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

SecurityLabelsCatalog::Item QtChatWindow::getSelectedSecurityLabel() {
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
		input_->setFocus();
		onAllMessagesRead();
	}
	
}

void QtChatWindow::setInputEnabled(bool enabled) {
	inputEnabled_ = enabled;
//	input_->setEnabled(enabled);
}

void QtChatWindow::showEvent(QShowEvent* event) {
	emit windowOpening();
	QWidget::showEvent(event);
}

void QtChatWindow::setUnreadMessageCount(int count) {
	if (unreadCount_ != count) {
		unreadCount_ = count;
		updateTitleWithUnreadCount();
		emit countUpdated();
	}
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

void QtChatWindow::setName(const std::string& name) {
	contact_ = P2QSTRING(name);
	updateTitleWithUnreadCount();
}

void QtChatWindow::updateTitleWithUnreadCount() {
	if (isWindow()) {
		setWindowTitle(unreadCount_ > 0 ? QString("(%1) %2").arg(unreadCount_).arg(contact_) : contact_);
	} else {
		setWindowTitle(contact_);
	}
	emit titleUpdated();
}

std::string QtChatWindow::addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) {
	return addMessage(message, senderName, senderIsSelf, label, avatarPath, "", time);
}

std::string QtChatWindow::addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString scaledAvatarPath = QtScaledAvatarCache(32).getScaledAvatarPath(avatarPath.c_str());

	QString htmlString;
	if (label) {
		htmlString = QString("<span style=\"border: thin dashed grey; padding-left: .5em; padding-right: .5em; color: %1; background-color: %2; font-size: 90%; margin-right: .5em; \">").arg(Qt::escape(P2QSTRING(label->getForegroundColor()))).arg(Qt::escape(P2QSTRING(label->getBackgroundColor())));
		htmlString += QString("%3</span> ").arg(Qt::escape(P2QSTRING(label->getDisplayMarking())));
	}
	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
	messageHTML.replace("\n","<br/>");
	QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
	QString styleSpanEnd = style == "" ? "" : "</span>";
	htmlString += styleSpanStart + messageHTML + styleSpanEnd;

	bool appendToPrevious = !previousMessageWasSystem_ && !previousMessageWasPresence_ && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_ && previousSenderName_ == P2QSTRING(senderName)));
	QString qAvatarPath =  scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
	std::string id = id_.generateID();
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), B2QDATE(time), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageWasSystem_ = false;
	previousMessageWasPresence_ = false;
	return id;
}

void QtChatWindow::flash() {
	emit requestFlash();
}

void QtChatWindow::setAckState(std::string const& id, ChatWindow::AckState state) {
	QString xml;
	switch (state) {
		case ChatWindow::Pending: xml = "<img src='qrc:/icons/throbber.gif' alt='" + tr("This message has not been received by your server yet.") + "'/>"; break;
		case ChatWindow::Received: xml = ""; break;
		case ChatWindow::Failed: xml = "<img src='qrc:/icons/error.png' alt='" + tr("This message may not have been transmitted.") + "'/>"; break;
	}
	messageLog_->setAckXML(P2QSTRING(id), xml);
}

int QtChatWindow::getCount() {
	return unreadCount_;
}

std::string QtChatWindow::addAction(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) {
	return addMessage(" *" + message + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ", time);
}

void QtChatWindow::addErrorMessage(const std::string& errorMessage) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString errorMessageHTML(Qt::escape(P2QSTRING(errorMessage)));
	errorMessageHTML.replace("\n","<br/>");
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(QString("<span class=\"error\">" + tr("Couldn't send message: %1") + "</span>").arg(errorMessageHTML), QDateTime::currentDateTime(), false, theme_)));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
	previousMessageWasPresence_ = false;
}

void QtChatWindow::addSystemMessage(const std::string& message) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
	messageHTML.replace("\n","<br/>");
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(), false, theme_)));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
	previousMessageWasPresence_ = false;
}

void QtChatWindow::addPresenceMessage(const std::string& message) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString messageHTML(Qt::escape(P2QSTRING(message)));
	messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
	messageHTML.replace("\n","<br/>");
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(), false, theme_)));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = false;
	previousMessageWasPresence_ = true;
}


void QtChatWindow::returnPressed() {
	if (!inputEnabled_) {
		return;
	}
	messageLog_->scrollToBottom();
	onSendMessageRequest(Q2PSTRING(input_->toPlainText()));
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
	if (isWindow()) {
		QWidget::show();
	}
	emit wantsToActivate();
	input_->setFocus();
}

void QtChatWindow::resizeEvent(QResizeEvent*) {
	emit geometryChanged();
}

void QtChatWindow::moveEvent(QMoveEvent*) {
	emit geometryChanged();	
}

void QtChatWindow::replaceLastMessage(const std::string& message) {
	messageLog_->replaceLastMessage(P2QSTRING(Linkify::linkify(message)));
}

}
