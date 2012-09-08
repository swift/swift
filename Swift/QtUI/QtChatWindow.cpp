/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindow.h"
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
#include "QtInviteToChatWindow.h"
#include <Swift/QtUI/QtUISettingConstants.h>

#include <Swiften/StringCodecs/Base64.h>
#include "SwifTools/TabComplete.h"
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include "QtChatWindowJSBridge.h"

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <QLabel>
#include <qdebug.h>
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
#include <QTextDocument>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swiften/Base/Log.h>

namespace Swift {

const QString QtChatWindow::ButtonWhiteboardSessionCancel = QString("whiteboard-cancel");
const QString QtChatWindow::ButtonWhiteboardSessionAcceptRequest = QString("whiteboard-acceptrequest");
const QString QtChatWindow::ButtonWhiteboardShowWindow = QString("whiteboard-showwindow");
const QString QtChatWindow::ButtonFileTransferCancel = QString("filetransfer-cancel");
const QString QtChatWindow::ButtonFileTransferSetDescription = QString("filetransfer-setdescription");
const QString QtChatWindow::ButtonFileTransferSendRequest = QString("filetransfer-sendrequest");
const QString QtChatWindow::ButtonFileTransferAcceptRequest = QString("filetransfer-acceptrequest");
const QString QtChatWindow::ButtonMUCInvite = QString("mucinvite");


QtChatWindow::QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings, QMap<QString, QString> emoticons) : QtTabbable(), contact_(contact), previousMessageWasSelf_(false), previousMessageKind_(PreviosuMessageWasNone), eventStream_(eventStream), emoticons_(emoticons) {
	settings_ = settings;
	unreadCount_ = 0;
	idCounter_ = 0;
	inputEnabled_ = true;
	completer_ = NULL;
	affiliationEditor_ = NULL;
	theme_ = theme;
	isCorrection_ = false;
	labelModel_ = NULL;
	correctionEnabled_ = Maybe;
	showEmoticons_ = true;
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

	midBar_ = new QWidget(this);
	//layout->addWidget(midBar);
	midBar_->setAutoFillBackground(true);
	QHBoxLayout *midBarLayout = new QHBoxLayout(midBar_);
	midBarLayout->setContentsMargins(0,0,0,0);
	midBarLayout->setSpacing(2);
	//midBarLayout->addStretch();

	labelsWidget_ = new QComboBox(this);
	labelsWidget_->setFocusPolicy(Qt::NoFocus);
	labelsWidget_->hide();
	labelsWidget_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	midBarLayout->addWidget(labelsWidget_,0);
	connect(labelsWidget_, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentLabelChanged(int)));
	defaultLabelsPalette_ = labelsWidget_->palette();

	QHBoxLayout* inputBarLayout = new QHBoxLayout();
	inputBarLayout->setContentsMargins(0,0,0,0);
	inputBarLayout->setSpacing(2);
	input_ = new QtTextEdit(this);
	input_->setAcceptRichText(false);
	inputBarLayout->addWidget(midBar_);
	inputBarLayout->addWidget(input_);
	correctingLabel_ = new QLabel(tr("Correcting"), this);
	inputBarLayout->addWidget(correctingLabel_);
	correctingLabel_->hide();
	layout->addLayout(inputBarLayout);

	inputClearing_ = false;
	contactIsTyping_ = false;
	tabCompletion_ = false;

	connect(input_, SIGNAL(unhandledKeyPressEvent(QKeyEvent*)), this, SLOT(handleKeyPressEvent(QKeyEvent*)));
	connect(input_, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	connect(input_, SIGNAL(textChanged()), this, SLOT(handleInputChanged()));
	connect(input_, SIGNAL(cursorPositionChanged()), this, SLOT(handleCursorPositionChanged()));
	setFocusProxy(input_);
	logRosterSplitter_->setFocusProxy(input_);
	midBar_->setFocusProxy(input_);
	messageLog_->setFocusProxy(input_);
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(qAppFocusChanged(QWidget*, QWidget*)));
	connect(messageLog_, SIGNAL(gotFocus()), input_, SLOT(setFocus()));
	resize(400,300);
	connect(messageLog_, SIGNAL(fontResized(int)), this, SIGNAL(fontResized(int)));
	connect(messageLog_, SIGNAL(logCleared()), this, SLOT(handleLogCleared()));

	treeWidget_->onSomethingSelectedChanged.connect(boost::bind(&QtChatWindow::handleOccupantSelectionChanged, this, _1));
	treeWidget_->onOccupantActionSelected.connect(boost::bind(boost::ref(onOccupantActionSelected), _1, _2));

	jsBridge = new QtChatWindowJSBridge();
	messageLog_->addToJSEnvironment("chatwindow", jsBridge);
	connect(jsBridge, SIGNAL(buttonClicked(QString,QString,QString,QString)), this, SLOT(handleHTMLButtonClicked(QString,QString,QString,QString)));

	settings_->onSettingChanged.connect(boost::bind(&QtChatWindow::handleSettingChanged, this, _1));
	showEmoticons_ = settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS);

}

QtChatWindow::~QtChatWindow() {
	delete jsBridge;
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
}

void QtChatWindow::handleSettingChanged(const std::string& setting) {
	if (setting == QtUISettingConstants::SHOW_EMOTICONS.getKey()) {
		showEmoticons_ = settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS);
		messageLog_->showEmoticons(showEmoticons_);
	}
}

void QtChatWindow::handleLogCleared() {
	onLogCleared();
}

void QtChatWindow::handleOccupantSelectionChanged(RosterItem* item) {
	onOccupantSelectionChanged(dynamic_cast<ContactRosterItem*>(item));
}

bool QtChatWindow::appendToPreviousCheck(QtChatWindow::PreviousMessageKind messageKind, const std::string& senderName, bool senderIsSelf) const {
	return previousMessageKind_ == messageKind && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_&& previousSenderName_ == P2QSTRING(senderName)));
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
	event->ignore();
	QtTabbable::handleKeyPressEvent(event);
	if (event->isAccepted()) {
		return;
	}
	event->accept();

	int key = event->key();
	if (key == Qt::Key_Tab) {
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
	labelsWidget_->setEnabled(false);
}

void QtChatWindow::cancelCorrection() {
	cancelAlert();
	QTextCursor cursor = input_->textCursor();
	cursor.select(QTextCursor::Document);
	cursor.removeSelectedText();
	isCorrection_ = false;
	correctingLabel_->hide();
	input_->setStyleSheet(qApp->styleSheet());
	labelsWidget_->setEnabled(true);
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

	QTextCursor cursor;
	if (tabCompleteCursor_.hasSelection()) {
		cursor = tabCompleteCursor_;
	} else {
		cursor = input_->textCursor();
		while(cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor) && cursor.document()->characterAt(cursor.position() - 1) != ' ') { }
	}
	QString root = cursor.selectedText();
	if (root.isEmpty()) {
		return;
	}
	QString suggestion = P2QSTRING(completer_->completeWord(Q2PSTRING(root)));
	if (root == suggestion) {
		return;
	}
	tabCompletion_ = true;
	cursor.beginEditBlock();
	cursor.removeSelectedText();
	int oldPosition = cursor.position();

	cursor.insertText(suggestion);
	tabCompleteCursor_ = cursor;
	tabCompleteCursor_.setPosition(oldPosition, QTextCursor::KeepAnchor);

	cursor.endEditBlock();
	tabCompletion_ = false;
}

void QtChatWindow::setRosterModel(Roster* roster) {
	treeWidget_->setRosterModel(roster);
}

void QtChatWindow::setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {
	delete labelModel_;
	labelModel_ = new LabelModel();
	labelModel_->availableLabels_ = labels;
	int i = 0;
	int defaultIndex = 0;
	labelsWidget_->setModel(labelModel_);
	foreach (SecurityLabelsCatalog::Item label, labels) {
		if (label.getIsDefault()) {
			defaultIndex = i;
			break;
		}
		i++;
	}
	labelsWidget_->setCurrentIndex(defaultIndex);
}

void QtChatWindow::handleCurrentLabelChanged(int index) {
	if (static_cast<size_t>(index) >= labelModel_->availableLabels_.size()) {
		qDebug() << "User selected a label that doesn't exist";
		return;
	}
	const SecurityLabelsCatalog::Item& label = labelModel_->availableLabels_[index];
	if (label.getLabel()) {
		QPalette palette = labelsWidget_->palette();
		//palette.setColor(QPalette::Base, P2QSTRING(label.getLabel()->getBackgroundColor()));
		palette.setColor(labelsWidget_->backgroundRole(), P2QSTRING(label.getLabel()->getBackgroundColor()));
		palette.setColor(labelsWidget_->foregroundRole(), P2QSTRING(label.getLabel()->getForegroundColor()));
		labelsWidget_->setPalette(palette);
		midBar_->setPalette(palette);
		labelsWidget_->setAutoFillBackground(true);
	}
	else {
		labelsWidget_->setAutoFillBackground(false);
		labelsWidget_->setPalette(defaultLabelsPalette_);
		midBar_->setPalette(defaultLabelsPalette_);
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

void QtChatWindow::setCorrectionEnabled(Tristate enabled) {
	correctionEnabled_ = enabled;
}

SecurityLabelsCatalog::Item QtChatWindow::getSelectedSecurityLabel() {
	assert(labelsWidget_->isEnabled());
	assert(labelsWidget_->currentIndex() >= 0 && static_cast<size_t>(labelsWidget_->currentIndex()) < labelModel_->availableLabels_.size());
	return labelModel_->availableLabels_[labelsWidget_->currentIndex()];
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
	return addMessage(linkimoticonify(P2QSTRING(message)), senderName, senderIsSelf, label, avatarPath, "", time);
}

QString QtChatWindow::linkimoticonify(const QString& message) const {
	QString messageHTML(message);
	messageHTML = Qt::escape(messageHTML);
	QMapIterator<QString, QString> it(emoticons_);
	QString textStyle = showEmoticons_ ? "style='display:none'" : "";
	QString imageStyle = showEmoticons_ ? "" : "style='display:none'";
	if (messageHTML.length() < 500) {
		while (it.hasNext()) {
			it.next();
			messageHTML.replace(it.key(), "<span class='swift_emoticon_image' " + imageStyle + "><img src='" + it.value() + "'/></span><span class='swift_emoticon_text' " + textStyle + ">"+it.key() + "</span>");
		}
		messageHTML = P2QSTRING(Linkify::linkify(Q2PSTRING(messageHTML)));
	}
	messageHTML.replace("\n","<br/>");
	return messageHTML;
}

std::string QtChatWindow::addMessage(const QString &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}
	QString scaledAvatarPath = QtScaledAvatarCache(32).getScaledAvatarPath(avatarPath.c_str());

	QString htmlString;
	if (label) {
		htmlString = QString("<span style=\"border: thin dashed grey; padding-left: .5em; padding-right: .5em; color: %1; background-color: %2; font-size: 90%; margin-right: .5em; \" class='swift_label'>").arg(Qt::escape(P2QSTRING(label->getForegroundColor()))).arg(Qt::escape(P2QSTRING(label->getBackgroundColor())));
		htmlString += QString("%1</span> ").arg(Qt::escape(P2QSTRING(label->getDisplayMarking())));
	}
	QString messageHTML(message);
	QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
	QString styleSpanEnd = style == "" ? "" : "</span>";
	htmlString += "<span class='swift_inner_message'>" + styleSpanStart + messageHTML + styleSpanEnd + "</span>" ;

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasMessage, senderName, senderIsSelf);
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
		appendToPrevious = false;
	}
	QString qAvatarPath =  scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
	std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), B2QDATE(time), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasMessage;
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
	return addMessage(" *" + linkimoticonify(P2QSTRING(message)) + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ", time);
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

QString encodeButtonArgument(const QString& str) {
	return Qt::escape(P2QSTRING(Base64::encode(createByteArray(Q2PSTRING(str)))));
}

QString decodeButtonArgument(const QString& str) {
	return P2QSTRING(byteArrayToString(Base64::decode(Q2PSTRING(str))));
}

QString QtChatWindow::buildChatWindowButton(const QString& name, const QString& id, const QString& arg1, const QString& arg2, const QString& arg3) {
	QRegExp regex("[A-Za-z][A-Za-z0-9\\-\\_]+");
	Q_ASSERT(regex.exactMatch(id));
	QString html = QString("<input id='%2' type='submit' value='%1' onclick='chatwindow.buttonClicked(\"%2\", \"%3\", \"%4\", \"%5\");' />").arg(name).arg(id).arg(encodeButtonArgument(arg1)).arg(encodeButtonArgument(arg2)).arg(encodeButtonArgument(arg3));
	return html;
}

std::string QtChatWindow::addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes) {
	SWIFT_LOG(debug) << "addFileTransfer" << std::endl;
	QString ft_id = QString("ft%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));
	
	QString htmlString;
	QString formattedFileSize = P2QSTRING(formatSize(sizeInBytes));
	if (senderIsSelf) {
		// outgoing
		htmlString = tr("Send file") + ": " + P2QSTRING(filename) + " ( " + formattedFileSize + ") <br/>" +
			"<div id='" + ft_id + "'>" +
				buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
				buildChatWindowButton(tr("Set Description"), ButtonFileTransferSetDescription, ft_id) +
				buildChatWindowButton(tr("Send"), ButtonFileTransferSendRequest, ft_id) +
			"</div>";
	} else {
		// incoming
		htmlString = tr("Receiving file") + ": " + P2QSTRING(filename) + " ( " + formattedFileSize  + ") <br/>" +
			"<div id='" + ft_id + "'>" +
				buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
				buildChatWindowButton(tr("Accept"), ButtonFileTransferAcceptRequest, ft_id, P2QSTRING(filename)) +
			"</div>";
	}

	//addMessage(message, senderName, senderIsSelf, boost::shared_ptr<SecurityLabel>(), "", boost::posix_time::second_clock::local_time());

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasFileTransfer, senderName, senderIsSelf);
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
		appendToPrevious = false;
	}
	QString qAvatarPath = "qrc:/icons/avatar.png";
	std::string id = "ftmessage" + boost::lexical_cast<std::string>(idCounter_++);
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id))));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasFileTransfer;
	return Q2PSTRING(ft_id);
}

void QtChatWindow::setFileTransferProgress(std::string id, const int percentageDone) {
	messageLog_->setFileTransferProgress(QString::fromStdString(id), percentageDone);
}

void QtChatWindow::setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg) {
	messageLog_->setFileTransferStatus(QString::fromStdString(id), state, QString::fromStdString(msg));
}

std::string QtChatWindow::addWhiteboardRequest(bool senderIsSelf) {
	QString wb_id = QString("wb%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));
	QString htmlString;
	if (senderIsSelf) {
		htmlString = "<div id='" + wb_id + "'>" + tr("Starting whiteboard chat") + "<br />"+
				buildChatWindowButton(tr("Cancel"), ButtonWhiteboardSessionCancel, wb_id) +
			"</div>";
	} else {
		htmlString = "<div id='" + wb_id + "'>" + Qt::escape(contact_) + tr(" would like to start whiteboard chat") + ": <br/>" +
				buildChatWindowButton(tr("Cancel"), ButtonWhiteboardSessionCancel, wb_id) +
				buildChatWindowButton(tr("Accept"), ButtonWhiteboardSessionAcceptRequest, wb_id) +
			"</div>";
	}

	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
//		appendToPrevious = false;
	}
	QString qAvatarPath = "qrc:/icons/avatar.png";
	std::string id = "wbmessage" + boost::lexical_cast<std::string>(idCounter_++);
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(contact_), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, false, false, theme_, P2QSTRING(id))));

	previousMessageWasSelf_ = false;
	previousSenderName_ = contact_;
	return Q2PSTRING(wb_id);
}

void QtChatWindow::setWhiteboardSessionStatus(std::string id, const ChatWindow::WhiteboardSessionState state) {
	messageLog_->setWhiteboardSessionStatus(QString::fromStdString(id), state);
}

void QtChatWindow::handleHTMLButtonClicked(QString id, QString encodedArgument1, QString encodedArgument2, QString encodedArgument3) {
	QString arg1 = decodeButtonArgument(encodedArgument1);
	QString arg2 = decodeButtonArgument(encodedArgument2);
	QString arg3 = decodeButtonArgument(encodedArgument3);

	if (id.startsWith(ButtonFileTransferCancel)) {
		QString ft_id = arg1;
		onFileTransferCancel(Q2PSTRING(ft_id));
	}
	else if (id.startsWith(ButtonFileTransferSetDescription)) {
		QString ft_id = arg1;
		bool ok = false;
		QString text = QInputDialog::getText(this, tr("File transfer description"),
			tr("Description:"), QLineEdit::Normal, "", &ok);
		if (ok) {
			descriptions[ft_id] = text;
		}
	}
	else if (id.startsWith(ButtonFileTransferSendRequest)) {
		QString ft_id = arg1;
		QString text = descriptions.find(ft_id) == descriptions.end() ? QString() : descriptions[ft_id];
		onFileTransferStart(Q2PSTRING(ft_id), Q2PSTRING(text));
	}
	else if (id.startsWith(ButtonFileTransferAcceptRequest)) {
		QString ft_id = arg1;
		QString filename = arg2;

		QString path = QFileDialog::getSaveFileName(this, tr("Save File"), filename);
		if (!path.isEmpty()) {
			onFileTransferAccept(Q2PSTRING(ft_id), Q2PSTRING(path));
		}
	}
	else if (id.startsWith(ButtonWhiteboardSessionAcceptRequest)) {
		QString id = arg1;
		messageLog_->setWhiteboardSessionStatus(QString::fromStdString(Q2PSTRING(id)), ChatWindow::WhiteboardAccepted);
		onWhiteboardSessionAccept();
	}
	else if (id.startsWith(ButtonWhiteboardSessionCancel)) {
		QString id = arg1;
		messageLog_->setWhiteboardSessionStatus(QString::fromStdString(Q2PSTRING(id)), ChatWindow::WhiteboardTerminated);
		onWhiteboardSessionCancel();
	}
	else if (id.startsWith(ButtonWhiteboardShowWindow)) {
		QString id = arg1;
		onWhiteboardWindowShow();
	}
	else if (id.startsWith(ButtonMUCInvite)) {
		QString roomJID = arg1;
		QString password = arg2;
		QString elementID = arg3;

		eventStream_->send(boost::make_shared<JoinMUCUIEvent>(Q2PSTRING(roomJID), Q2PSTRING(password)));
		messageLog_->setMUCInvitationJoined(elementID);
	}
	else {
		SWIFT_LOG(debug) << "Unknown HTML button! ( " << Q2PSTRING(id) << " )" << std::endl;
	}
}

void QtChatWindow::addErrorMessage(const std::string& errorMessage) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString errorMessageHTML(Qt::escape(P2QSTRING(errorMessage)));
	errorMessageHTML.replace("\n","<br/>");
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet("<span class=\"error\">" + errorMessageHTML + "</span>", QDateTime::currentDateTime(), false, theme_)));

	previousMessageWasSelf_ = false;
	previousMessageKind_ = PreviousMessageWasSystem;
}

void QtChatWindow::addSystemMessage(const std::string& message) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString messageHTML(P2QSTRING(message));
	messageHTML = linkimoticonify(messageHTML);
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(), false, theme_)));

	previousMessageKind_ = PreviousMessageWasSystem;
}

void QtChatWindow::replaceWithAction(const std::string& message, const std::string& id, const boost::posix_time::ptime& time) {
	replaceMessage(" *" + linkimoticonify(P2QSTRING(message)) + "*", id, time, "font-style:italic ");
}

void QtChatWindow::replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time) {
	replaceMessage(linkimoticonify(P2QSTRING(message)), id, time, "");
}

void QtChatWindow::replaceMessage(const QString& message, const std::string& id, const boost::posix_time::ptime& time, const QString& style) {
	if (!id.empty()) {
		if (isWidgetSelected()) {
			onAllMessagesRead();
		}

		QString messageHTML(message);

		QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
		QString styleSpanEnd = style == "" ? "" : "</span>";
		messageHTML = styleSpanStart + messageHTML + styleSpanEnd;

		messageLog_->replaceMessage(messageHTML, P2QSTRING(id), B2QDATE(time));
	}
	else {
		std::cerr << "Trying to replace a message with no id";
	}
}

void QtChatWindow::addPresenceMessage(const std::string& message) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString messageHTML(P2QSTRING(message));
	messageHTML = linkimoticonify(messageHTML);
	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new SystemMessageSnippet(messageHTML, QDateTime::currentDateTime(), false, theme_)));

	previousMessageKind_ = PreviousMessageWasPresence;
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

void QtChatWindow::handleCursorPositionChanged() {
	if (tabCompletion_) {
		return;
	}
	tabCompleteCursor_.clearSelection();
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
	messageLog_->replaceLastMessage(linkimoticonify(P2QSTRING(message)));
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
	QAction* changeSubject = NULL;
	QAction* configure = NULL;
	QAction* affiliations = NULL;
	QAction* destroy = NULL;
	QAction* invite = NULL;

	foreach(ChatWindow::RoomAction availableAction, availableRoomActions_)
	{
		switch(availableAction)
		{
			case ChatWindow::ChangeSubject: changeSubject = contextMenu.addAction(tr("Change subject…")); break;
			case ChatWindow::Configure: configure = contextMenu.addAction(tr("Configure room…")); break;
			case ChatWindow::Affiliations: affiliations = contextMenu.addAction(tr("Edit affiliations…")); break;
			case ChatWindow::Destroy: destroy = contextMenu.addAction(tr("Destroy room")); break;
			case ChatWindow::Invite: invite = contextMenu.addAction(tr("Invite person to this room…")); break;
		}
	}

	QAction* result = contextMenu.exec(QCursor::pos());
	if (result == NULL) {
		/* Skip processing. Note that otherwise, because the actions could be null they could match */
	}
	else if (result == changeSubject) {
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
		onInvitePersonToThisMUCRequest();
	}
}

void QtChatWindow::handleAffiliationEditorAccepted() {
	onChangeAffiliationsRequest(affiliationEditor_->getChanges());
}

void QtChatWindow::setAffiliations(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
	if (!affiliationEditor_) return;
	affiliationEditor_->setAffiliations(affiliation, jids);
}

void QtChatWindow::setAvailableRoomActions(const std::vector<RoomAction> &actions)
{
	availableRoomActions_ = actions;
}

void QtChatWindow::showRoomConfigurationForm(Form::ref form) {
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
	mucConfigurationWindow_ = new QtMUCConfigurationWindow(form);
	mucConfigurationWindow_->onFormComplete.connect(boost::bind(boost::ref(onConfigureRequest), _1));
	mucConfigurationWindow_->onFormCancelled.connect(boost::bind(boost::ref(onConfigurationFormCancelled)));
}

void QtChatWindow::addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct) {
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}

	QString htmlString = QObject::tr("You've been invited to enter the %1 room.").arg(P2QSTRING(jid.toString())) + " <br/>";
	if (!reason.empty()) {
		htmlString += QObject::tr("Reason: %1").arg(P2QSTRING(reason)) + "<br/>";
	}
	if (!direct) {
		htmlString += QObject::tr("This person may not have really sent this invitation!") + "<br/>";
	}

	QString id = QString(ButtonMUCInvite + "%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));

	htmlString += "<div id='" + id + "'>" +
			buildChatWindowButton(tr("Accept Invite"), ButtonMUCInvite, Qt::escape(P2QSTRING(jid.toString())), Qt::escape(P2QSTRING(password)), id) +
		"</div>";

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasMUCInvite, senderName, false);
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
		/* if the line is added we should break the snippet */
		appendToPrevious = false;
	}
	QString qAvatarPath = "qrc:/icons/avatar.png";

	messageLog_->addMessageBottom(boost::shared_ptr<ChatSnippet>(new MessageSnippet(htmlString, Qt::escape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, false, appendToPrevious, theme_, id)));
	previousMessageWasSelf_ = false;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasMUCInvite;
}


InviteToChatWindow* QtChatWindow::createInviteToChatWindow() {
	return new QtInviteToChatWindow(this);
}


}
