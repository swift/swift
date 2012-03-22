/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/RosterItem.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Roster/QtOccupantListWidget.h"
#include "SwifTools/Linkify.h"
#include "QtChatView.h"
#include "MessageSnippet.h"
#include "SystemMessageSnippet.h"
#include "QtTextEdit.h"
#include "QtSettingsProvider.h"
#include "QtScaledAvatarCache.h"

#include "SwifTools/TabComplete.h"
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include "QtFileTransferJSBridge.h"

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
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
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>
#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <QDebug>

namespace Swift {
QtChatWindow::QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings) : QtTabbable(), contact_(contact), previousMessageWasSelf_(false), previousMessageWasSystem_(false), previousMessageWasPresence_(false), previousMessageWasFileTransfer_(false), eventStream_(eventStream) {
	settings_ = settings;
	unreadCount_ = 0;
	idCounter_ = 0;
	inputEnabled_ = true;
	completer_ = NULL;
	affiliationEditor_ = NULL;
	theme_ = theme;
	isCorrection_ = false;
	correctionEnabled_ = Maybe;
	updateTitleWithUnreadCount();

#ifdef SWIFT_EXPERIMENTAL_FT
	setAcceptDrops(true);
#endif

	alertStyleSheet_ = "background: rgb(255, 255, 153); color: black";

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);

	alertWidget_ = new QWidget(this);
	QHBoxLayout* alertLayout = new QHBoxLayout(alertWidget_);
	layout->addWidget(alertWidget_);
	alertLabel_ = new QLabel(this);
	alertLayout->addWidget(alertLabel_);
	alertButton_ = new QPushButton(this);
	connect (alertButton_, SIGNAL(clicked()), this, SLOT(handleAlertButtonClicked()));
	alertLayout->addWidget(alertButton_);
	QPalette palette = alertWidget_->palette();
	palette.setColor(QPalette::Window, QColor(Qt::yellow));
	palette.setColor(QPalette::WindowText, QColor(Qt::black));
	alertWidget_->setStyleSheet(alertStyleSheet_);
	alertLabel_->setStyleSheet(alertStyleSheet_);
	alertWidget_->hide();

	QBoxLayout* subjectLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	subject_ = new QLineEdit(this);
	subjectLayout->addWidget(subject_);
	setSubject("");
	subject_->setReadOnly(true);

	actionButton_ = new QPushButton(this);
	actionButton_->setIcon(QIcon(":/icons/actions.png"));
	connect(actionButton_, SIGNAL(clicked()), this, SLOT(handleActionButtonClicked()));
	subjectLayout->addWidget(actionButton_);

	subject_->hide();
	actionButton_->hide();

	layout->addLayout(subjectLayout);

	logRosterSplitter_ = new QSplitter(this);
	logRosterSplitter_->setAutoFillBackground(true);
	layout->addWidget(logRosterSplitter_);
	messageLog_ = new QtChatView(theme, this);
	logRosterSplitter_->addWidget(messageLog_);

	treeWidget_ = new QtOccupantListWidget(eventStream_, settings_, this);
	treeWidget_->hide();
	logRosterSplitter_->addWidget(treeWidget_);
	logRosterSplitter_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(logRosterSplitter_, SIGNAL(splitterMoved(int, int)), this, SLOT(handleSplitterMoved(int, int)));

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

	QHBoxLayout* inputBarLayout = new QHBoxLayout();
	inputBarLayout->setContentsMargins(0,0,0,0);
	inputBarLayout->setSpacing(2);
	input_ = new QtTextEdit(this);
	input_->setAcceptRichText(false);
	inputBarLayout->addWidget(input_);
	correctingLabel_ = new QLabel(tr("Correcting"), this);
	inputBarLayout->addWidget(correctingLabel_);
	correctingLabel_->hide();
	layout->addLayout(inputBarLayout);

	inputClearing_ = false;
	contactIsTyping_ = false;

	connect(input_, SIGNAL(unhandledKeyPressEvent(QKeyEvent*)), this, SLOT(handleKeyPressEvent(QKeyEvent*)));
	connect(input_, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	connect(input_, SIGNAL(textChanged()), this, SLOT(handleInputChanged()));
	setFocusProxy(input_);
	logRosterSplitter_->setFocusProxy(input_);
	midBar->setFocusProxy(input_);
	messageLog_->setFocusProxy(input_);
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(qAppFocusChanged(QWidget*, QWidget*)));
	connect(messageLog_, SIGNAL(gotFocus()), input_, SLOT(setFocus()));
	resize(400,300);
	connect(messageLog_, SIGNAL(fontResized(int)), this, SIGNAL(fontResized(int)));

	treeWidget_->onSomethingSelectedChanged.connect(boost::bind(&QtChatWindow::handleOccupantSelectionChanged, this, _1));
	treeWidget_->onOccupantActionSelected.connect(boost::bind(boost::ref(onOccupantActionSelected), _1, _2));

	fileTransferJS = new QtFileTransferJSBridge();
	messageLog_->addToJSEnvironment("filetransfer", fileTransferJS);
	connect(fileTransferJS, SIGNAL(setDescription(QString)), this, SLOT(handleFileTransferSetDescription(QString)));
	connect(fileTransferJS, SIGNAL(sendRequest(QString)), this, SLOT(handleFileTransferStart(QString)));
	connect(fileTransferJS, SIGNAL(acceptRequest(QString, QString)), this, SLOT(handleFileTransferAccept(QString, QString)));
	connect(fileTransferJS, SIGNAL(cancel(QString)), this, SLOT(handleFileTransferCancel(QString)));
}

QtChatWindow::~QtChatWindow() {
	delete fileTransferJS;
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
}


void QtChatWindow::handleOccupantSelectionChanged(RosterItem* item) {
	onOccupantSelectionChanged(dynamic_cast<ContactRosterItem*>(item));
}

void QtChatWindow::handleFontResized(int fontSizeSteps) {
	messageLog_->resizeFont(fontSizeSteps);
}

void QtChatWindow::handleAlertButtonClicked() {
	onAlertButtonClicked();
}

void QtChatWindow::setAlert(const std::string& alertText, const std::string& buttonText) {
	alertLabel_->setText(alertText.c_str());
	if (buttonText.empty()) {
		alertButton_->hide();
	} else {
		alertButton_->setText(buttonText.c_str());
		alertButton_->show();
	}
	alertWidget_->show();
}

void QtChatWindow::cancelAlert() {
	alertWidget_->hide();
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
	} else if ((key == Qt::Key_Up) && input_->toPlainText().isEmpty() && !(lastSentMessage_.isEmpty())) {
		beginCorrection();
	} else if (key == Qt::Key_Down && isCorrection_ && input_->textCursor().atBlockEnd()) {
		cancelCorrection();
	} else if (key == Qt::Key_Down || key == Qt::Key_Up) {
		/* Drop */
	} else {
		messageLog_->handleKeyPressEvent(event);
	}
}

void QtChatWindow::beginCorrection() {
	if (correctionEnabled_ == ChatWindow::Maybe) {
		setAlert(Q2PSTRING(tr("This chat may not support message correction. If you send a correction anyway, it may appear as a duplicate message")));
	} else if (correctionEnabled_ == ChatWindow::No) {
		setAlert(Q2PSTRING(tr("This chat does not support message correction.  If you send a correction anyway, it will appear as a duplicate message")));
	}
	QTextCursor cursor = input_->textCursor();
	cursor.select(QTextCursor::Document);
	cursor.beginEditBlock();
	cursor.insertText(QString(lastSentMessage_));
	cursor.endEditBlock();
	isCorrection_ = true;
	correctingLabel_->show();
	input_->setStyleSheet(alertStyleSheet_);
}

void QtChatWindow::cancelCorrection() {
	cancelAlert();
	QTextCursor cursor = input_->textCursor();
	cursor.select(QTextCursor::Document);
	cursor.removeSelectedText();
	isCorrection_ = false;
	correctingLabel_->hide();
	input_->setStyleSheet(qApp->styleSheet());
}

QByteArray QtChatWindow::getSplitterState() {
	return logRosterSplitter_->saveState();
}

void QtChatWindow::handleChangeSplitterState(QByteArray state) {
	logRosterSplitter_->restoreState(state);
}

void QtChatWindow::handleSplitterMoved(int, int) {
	emit splitterMoved();
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

void QtChatWindow::setCorrectionEnabled(Tristate enabled) {
	correctionEnabled_ = enabled;
}

SecurityLabelsCatalog::Item QtChatWindow::getSelectedSecurityLabel() {
	assert(labelsWidget_->isEnabled());
	assert(labelsWidget_->currentIndex() >= 0 && static_cast<size_t>(labelsWidget_->currentIndex()) < availableLabels_.size());
	return availableLabels_[labelsWidget_->currentIndex()];
}

void QtChatWindow::closeEvent(QCloseEvent* event) {
	event->accept();
	emit windowClosing();
	onClosed();
}

void QtChatWindow::convertToMUC() {
	setAcceptDrops(false);
	treeWidget_->show();
	subject_->show();
	actionButton_->show();
}

void QtChatWindow::qAppFocusChanged(QWidget* /*old*/, QWidget* /*now*/) {
	if (isWidgetSelected()) {
		lastLineTracker_.setHasFocus(true);
		input_->setFocus();
		onAllMessagesRead();
	}
	else {
		lastLineTracker_.setHasFocus(false);
	}
}

void QtChatWindow::setInputEnabled(bool enabled) {
	inputEnabled_ = enabled;
	if (!enabled) {
		if (mucConfigurationWindow_) {
			delete mucConfigurationWindow_.data();
		}
		if (affiliationEditor_) {
			delete affiliationEditor_.data();
		}
	}
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

	bool appendToPrevious = !previousMessageWasFileTransfer_ && !previousMessageWasSystem_ && !previousMessageWasPresence_ && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_ && previousSenderName_ == P2QSTRING(senderName)));
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
		appendToPrevious = false;
	}
	QString qAvatarPath =  scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
	std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), B2QDATE(time), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageWasSystem_ = false;
	previousMessageWasPresence_ = false;
	previousMessageWasFileTransfer_ = false;
	return id;
}

void QtChatWindow::flash() {
	emit requestFlash();
}

void QtChatWindow::setAckState(std::string const& id, ChatWindow::AckState state) {
	QString xml;
	switch (state) {
		case ChatWindow::Pending:
			xml = "<img src='qrc:/icons/throbber.gif' title='" + tr("This message has not been received by your server yet.") + "'/>";
			messageLog_->displayReceiptInfo(P2QSTRING(id), false);
			break;
		case ChatWindow::Received:
			xml = "";
			messageLog_->displayReceiptInfo(P2QSTRING(id), true);
			break;
		case ChatWindow::Failed: xml = "<img src='qrc:/icons/error.png' title='" + tr("This message may not have been transmitted.") + "'/>"; break;
	}
	messageLog_->setAckXML(P2QSTRING(id), xml);
}

void QtChatWindow::setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) {
	QString xml;
	switch (state) {
		case ChatWindow::ReceiptReceived:
			xml = "<img src='qrc:/icons/check.png' title='" + tr("The receipt for this message has been received.") + "'/>";
			break;
		case ChatWindow::ReceiptRequested:
			xml = "<img src='qrc:/icons/warn.png' title='" + tr("The receipt for this message has not yet been received. The recipient(s) might not have received this message.") + "'/>";
			break;
	}
	messageLog_->setReceiptXML(P2QSTRING(id), xml);
}

int QtChatWindow::getCount() {
	return unreadCount_;
}

std::string QtChatWindow::addAction(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) {
	return addMessage(" *" + message + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ", time);
}

std::string formatSize(const boost::uintmax_t bytes) {
	static const char *siPrefix[] = {"k", "M", "G", "T", "P", "E", "Z", "Y", NULL};
	int power = 0;
	double engBytes = bytes;
	while (engBytes >= 1000) {
		++power;
		engBytes = engBytes / 1000.0;
	}
	return str( boost::format("%.1lf %sB") % engBytes % (power > 0 ? siPrefix[power-1] : "") );
}

std::string QtChatWindow::addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes) {
	qDebug() << "addFileTransfer";
	std::string ft_id = "ft" + boost::lexical_cast<std::string>(idCounter_++);
	
	std::string htmlString;
	if (senderIsSelf) {
		// outgoing
		htmlString = "Send file: " + filename + " ( " + formatSize(sizeInBytes) + ") </br>" +
			"<div id='" + ft_id + "'>" +
			"<input id='discard' type='submit' value='Cancel' onclick='filetransfer.cancel(\"" + ft_id + "\");' />" +
			"<input id='description' type='submit' value='Set Description' onclick='filetransfer.setDescription(\"" + ft_id + "\");' />" +
			"<input id='send' type='submit' value='Send' onclick='filetransfer.sendRequest(\"" + ft_id + "\");' />" +
			"</div>";
	} else {
		// incoming
		htmlString = "Receiving file: " + filename + " ( " + formatSize(sizeInBytes)  + ") </br>" +
			"<div id='" + ft_id + "'>" +
			"<input id='discard' type='submit' value='Cancel' onclick='filetransfer.cancel(\"" + ft_id + "\");' />" +
			"<input id='accept' type='submit' value='Accept' onclick='filetransfer.acceptRequest(\"" + ft_id + "\", \"" + filename + "\");' />" +
			"</div>";
	}

	//addMessage(message, senderName, senderIsSelf, boost::shared_ptr<SecurityLabel>(), "", boost::posix_time::second_clock::local_time());

	bool appendToPrevious = !previousMessageWasFileTransfer_ && !previousMessageWasSystem_ && !previousMessageWasPresence_ && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_ && previousSenderName_ == P2QSTRING(senderName)));
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
		appendToPrevious = false;
	}
	QString qAvatarPath = "qrc:/icons/avatar.png";
	std::string id = "ftmessage" + boost::lexical_cast<std::string>(idCounter_++);
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new MessageSnippet(QString::fromStdString(htmlString), Qt::escape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));


	return ft_id;
}

void QtChatWindow::setFileTransferProgress(std::string id, const int percentageDone) {
	messageLog_->setFileTransferProgress(QString::fromStdString(id), percentageDone);
}

void QtChatWindow::setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg) {
	messageLog_->setFileTransferStatus(QString::fromStdString(id), state, QString::fromStdString(msg));
}

void QtChatWindow::handleFileTransferCancel(QString id) {
	qDebug() << "QtChatWindow::handleFileTransferCancel(" << id << ")";
	onFileTransferCancel(Q2PSTRING(id));
}

void QtChatWindow::handleFileTransferSetDescription(QString id) {
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("File transfer description"),
		tr("Description:"), QLineEdit::Normal, "", &ok);
	if (ok) {
		descriptions[id] = text;
	}
}

void QtChatWindow::handleFileTransferStart(QString id) {
	qDebug() << "QtChatWindow::handleFileTransferStart(" << id << ")";
	
	QString text = descriptions.find(id) == descriptions.end() ? QString() : descriptions[id];
	onFileTransferStart(Q2PSTRING(id), Q2PSTRING(text));
}

void QtChatWindow::handleFileTransferAccept(QString id, QString filename) {
	qDebug() << "QtChatWindow::handleFileTransferAccept(" << id << ", " << filename << ")";

	QString path = QFileDialog::getSaveFileName(this, tr("Save File"), filename);
	if (!path.isEmpty()) {
		onFileTransferAccept(Q2PSTRING(id), Q2PSTRING(path));
	}
}

void QtChatWindow::addErrorMessage(const std::string& errorMessage) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString errorMessageHTML(Qt::escape(P2QSTRING(errorMessage)));
	errorMessageHTML.replace("\n","<br/>");
	messageLog_->addMessage(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet("<span class=\"error\">" + errorMessageHTML + "</span>", QDateTime::currentDateTime(), false, theme_)));

	previousMessageWasSelf_ = false;
	previousMessageWasSystem_ = true;
	previousMessageWasPresence_ = false;
	previousMessageWasFileTransfer_ = false;
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
	previousMessageWasFileTransfer_ = false;
}

void QtChatWindow::replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time) {
	if (!id.empty()) {
		QString messageHTML(Qt::escape(P2QSTRING(message)));
		messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
		messageHTML.replace("\n","<br/>");
		messageLog_->replaceMessage(messageHTML, P2QSTRING(id), B2QDATE(time));
	}
	else {
		qWarning() << "Trying to replace a message with no id";
	}
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
	lastSentMessage_ = QString(input_->toPlainText());
	onSendMessageRequest(Q2PSTRING(input_->toPlainText()), isCorrection_);
	inputClearing_ = true;
	input_->clear();
	cancelCorrection();
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
	if (parentWidget() == NULL) {
		QWidget::show();
	}
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

void QtChatWindow::dragEnterEvent(QDragEnterEvent *event) {
	if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
		// TODO: check whether contact actually supports file transfer
		event->acceptProposedAction();
	}
}

void QtChatWindow::dropEvent(QDropEvent *event) {
	if (event->mimeData()->urls().size() == 1) {
		onSendFileRequest(Q2PSTRING(event->mimeData()->urls().at(0).toLocalFile()));
	} else {
		addSystemMessage("Sending of multiple files at once isn't supported at this time.");
	}
}

void QtChatWindow::replaceLastMessage(const std::string& message) {
	messageLog_->replaceLastMessage(P2QSTRING(Linkify::linkify(message)));
}

void QtChatWindow::setAvailableOccupantActions(const std::vector<OccupantAction>& actions) {
	treeWidget_->setAvailableOccupantActions(actions);
}

void QtChatWindow::setSubject(const std::string& subject) {
	//subject_->setVisible(!subject.empty());
	subject_->setText(P2QSTRING(subject));
}

void QtChatWindow::handleActionButtonClicked() {
	QMenu contextMenu;
	QAction* changeSubject = contextMenu.addAction(tr("Change subject"));
	QAction* configure = contextMenu.addAction(tr("Configure room"));
	QAction* affiliations = contextMenu.addAction(tr("Edit affiliations"));
	QAction* destroy = contextMenu.addAction(tr("Destroy room"));
	QAction* invite = contextMenu.addAction(tr("Invite person to this room"));
	QAction* result = contextMenu.exec(QCursor::pos());
	if (result == changeSubject) {
		bool ok;
		QString subject = QInputDialog::getText(this, tr("Change room subject"), tr("New subject:"), QLineEdit::Normal, subject_->text(), &ok);
		if (ok) {
			onChangeSubjectRequest(Q2PSTRING(subject));
		}
	}
	else if (result == configure) {
		onConfigureRequest(Form::ref());
	}
	else if (result == affiliations) {
		if (!affiliationEditor_) {
			onGetAffiliationsRequest();
			affiliationEditor_ = new QtAffiliationEditor(this);
			connect(affiliationEditor_, SIGNAL(accepted()), this, SLOT(handleAffiliationEditorAccepted()));
		}
		affiliationEditor_->show();
	}
	else if (result == destroy) {
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Confirm room destruction"));
		msgBox.setText(tr("Are you sure you want to destroy the room?"));
		msgBox.setInformativeText(tr("This will destroy the room."));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		if (msgBox.exec() == QMessageBox::Yes) {
			onDestroyRequest();
		}
	}
	else if (result == invite) {
		bool ok;
		QString jid = QInputDialog::getText(this, tr("Enter person's address"), tr("Address:"), QLineEdit::Normal, "", &ok);
		if (ok) {
			onInvitePersonToThisMUCRequest(JID(Q2PSTRING(jid)), "");
		}
	}
}

void QtChatWindow::handleAffiliationEditorAccepted() {
	onChangeAffiliationsRequest(affiliationEditor_->getChanges());
}

void QtChatWindow::setAffiliations(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
	if (!affiliationEditor_) return;
	affiliationEditor_->setAffiliations(affiliation, jids);
}

void QtChatWindow::showRoomConfigurationForm(Form::ref form) {
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
	mucConfigurationWindow_ = new QtMUCConfigurationWindow(form);
	mucConfigurationWindow_->onFormComplete.connect(boost::bind(boost::ref(onConfigureRequest), _1));
	mucConfigurationWindow_->onFormCancelled.connect(boost::bind(boost::ref(onConfigurationFormCancelled)));
}

void QtChatWindow::addMUCInvitation(const JID& jid, const std::string& reason, const std::string& /*password*/, bool direct) {
	bool accepted = false;
	QMessageBox msgBox;
	//FIXME: horrid modal untranslated popup. Fix before release.
	msgBox.setText(QString("You have been invited to the room %1 by %2.").arg(P2QSTRING(jid.toString())).arg(contact_));
	QString reasonString;
	if (!reason.empty()) {
		reasonString = QString("\"%1\"").arg(P2QSTRING(reason));
	}
	if (!direct) {
		if (!reasonString.isEmpty()) {
			reasonString += " ";
		}
		reasonString += QString(" (%1 may not have really sent this invitation)").arg(contact_);
	}
	msgBox.setInformativeText(QString("Accept invitation from %1 to enter %2?\n%3").arg(contact_).arg(P2QSTRING(jid.toString())).arg(reasonString));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	switch (ret) {
	   case QMessageBox::Yes:
	       accepted = true;
	       break;
	   default:
	       break;
	 }
	if (accepted) {
		eventStream_->send(boost::make_shared<JoinMUCUIEvent>(jid));
	}
}

}
