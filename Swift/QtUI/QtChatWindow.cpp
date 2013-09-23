/*
 * Copyright (c) 2010-2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatWindow.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/RosterItem.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Roster/QtOccupantListWidget.h"
#include "SwifTools/Linkify.h"
#include "QtWebKitChatView.h"
#include "QtTextEdit.h"
#include "QtSettingsProvider.h"
#include "QtScaledAvatarCache.h"
#include <Swift/QtUI/QtUISettingConstants.h>

#include "SwifTools/TabComplete.h"
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include "QtUtilities.h"

#include <boost/cstdint.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <QLabel>
#include <qdebug.h>
#include <QMessageBox>
#include <QMimeData>
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
#include <QToolButton>
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>
#include <QTextDocument>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swiften/Base/Log.h>

namespace Swift {

QtChatWindow::QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings) : QtTabbable(), contact_(contact), eventStream_(eventStream), blockingState_(BlockingUnsupported), isMUC_(false), supportsImpromptuChat_(false) {
	settings_ = settings;
	unreadCount_ = 0;
	inputEnabled_ = true;
	completer_ = NULL;
	affiliationEditor_ = NULL;
	theme_ = theme;
	isCorrection_ = false;
	labelModel_ = NULL;
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

	subjectLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	subject_ = new QLineEdit(this);
	subjectLayout_->addWidget(subject_);
	setSubject("");
	subject_->setReadOnly(true);

	QPushButton* actionButton_ = new QPushButton(this);
	actionButton_->setIcon(QIcon(":/icons/actions.png"));
	connect(actionButton_, SIGNAL(clicked()), this, SLOT(handleActionButtonClicked()));
	subject_->hide();

	layout->addLayout(subjectLayout_);

	logRosterSplitter_ = new QSplitter(this);
	logRosterSplitter_->setAutoFillBackground(true);
	layout->addWidget(logRosterSplitter_);
	messageLog_ = new QtWebKitChatView(this, eventStream_, theme, this); // I accept that passing the ChatWindow in so that the view can call the signals is somewhat inelegant, but it saves a lot of boilerplate. This patch is unpleasant enough already. So let's fix this soon (it at least needs fixing by the time history is sorted), but not now.
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
	input_ = new QtTextEdit(settings_, this);
	input_->setAcceptRichText(false);
	inputBarLayout->addWidget(midBar_);
	inputBarLayout->addWidget(input_);
	correctingLabel_ = new QLabel(tr("Correcting"), this);
	inputBarLayout->addWidget(correctingLabel_);
	correctingLabel_->hide();

	// using an extra layout to work around Qt margin glitches on OS X
	QHBoxLayout* actionLayout = new QHBoxLayout();
	actionLayout->addWidget(actionButton_);

	inputBarLayout->addLayout(actionLayout);
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

	settings_->onSettingChanged.connect(boost::bind(&QtChatWindow::handleSettingChanged, this, _1));
	messageLog_->showEmoticons(settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS));

}

QtChatWindow::~QtChatWindow() {
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
}

void QtChatWindow::handleSettingChanged(const std::string& setting) {
	if (setting == QtUISettingConstants::SHOW_EMOTICONS.getKey()) {
		bool showEmoticons = settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS);
		messageLog_->showEmoticons(showEmoticons);
	}
}

void QtChatWindow::handleLogCleared() {
	onLogCleared();
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

void QtChatWindow::convertToMUC(bool impromptuMUC) {
	impromptu_ = impromptuMUC;
	isMUC_ = true;
	treeWidget_->show();
	subject_->setVisible(!impromptu_);
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



void QtChatWindow::flash() {
	emit requestFlash();
}

int QtChatWindow::getCount() {
	return unreadCount_;
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
		if (!isMUC_) {
			event->acceptProposedAction();
		}
	} else if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid")) {
		if (isMUC_ || supportsImpromptuChat_) {
			event->acceptProposedAction();
		}
	}
}

void QtChatWindow::dropEvent(QDropEvent *event) {
	if (event->mimeData()->hasUrls()) {
		if (event->mimeData()->urls().size() == 1) {
			onSendFileRequest(Q2PSTRING(event->mimeData()->urls().at(0).toLocalFile()));
		} else {
			std::string messageText(Q2PSTRING(tr("Sending of multiple files at once isn't supported at this time.")));
			ChatMessage message;
			message.append(boost::make_shared<ChatTextMessagePart>(messageText));
			addSystemMessage(message, DefaultDirection);
		}
	} else if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid")) {
		QByteArray dataBytes = event->mimeData()->data("application/vnd.swift.contact-jid");
		QDataStream dataStream(&dataBytes, QIODevice::ReadOnly);
		QString jidString;
		dataStream >> jidString;
		onInviteToChat(std::vector<JID>(1, JID(Q2PSTRING(jidString))));
	}
}


void QtChatWindow::setAvailableOccupantActions(const std::vector<OccupantAction>& actions) {
	treeWidget_->setAvailableOccupantActions(actions);
}

void QtChatWindow::setSubject(const std::string& subject) {
	//subject_->setVisible(!subject.empty());
	subject_->setText(P2QSTRING(subject));
	subject_->setToolTip(P2QSTRING(subject));
	subject_->setCursorPosition(0);
}

void QtChatWindow::handleActionButtonClicked() {
	QMenu contextMenu;
	QAction* changeSubject = NULL;
	QAction* configure = NULL;
	QAction* affiliations = NULL;
	QAction* destroy = NULL;
	QAction* invite = NULL;

	QAction* block = NULL;
	QAction* unblock = NULL;

	if (availableRoomActions_.empty()) {
		if (blockingState_ == IsBlocked) {
			unblock = contextMenu.addAction(tr("Unblock"));
		} else if (blockingState_ == IsUnblocked) {
			block = contextMenu.addAction(tr("Block"));
		}

		if (supportsImpromptuChat_) {
			invite = contextMenu.addAction(tr("Invite person to this chat…"));
		}

	} else {
		foreach(ChatWindow::RoomAction availableAction, availableRoomActions_)
		{
			if (impromptu_) {
				// hide options we don't need in impromptu chats
				if (availableAction == ChatWindow::ChangeSubject ||
					availableAction == ChatWindow::Configure ||
					availableAction == ChatWindow::Affiliations ||
					availableAction == ChatWindow::Destroy) {
					continue;
				}
			}
			switch(availableAction)
			{
				case ChatWindow::ChangeSubject: changeSubject = contextMenu.addAction(tr("Change subject…")); break;
				case ChatWindow::Configure: configure = contextMenu.addAction(tr("Configure room…")); break;
				case ChatWindow::Affiliations: affiliations = contextMenu.addAction(tr("Edit affiliations…")); break;
				case ChatWindow::Destroy: destroy = contextMenu.addAction(tr("Destroy room")); break;
				case ChatWindow::Invite: invite = contextMenu.addAction(tr("Invite person to this room…")); break;
			}
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
		onInviteToChat(std::vector<JID>());
	}
	else if (result == block) {
		onBlockUserRequest();
	}
	else if (result == unblock) {
		onUnblockUserRequest();
	}
}

void QtChatWindow::handleAffiliationEditorAccepted() {
	onChangeAffiliationsRequest(affiliationEditor_->getChanges());
}

void QtChatWindow::setAffiliations(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
	if (!affiliationEditor_) return;
	affiliationEditor_->setAffiliations(affiliation, jids);
}

void QtChatWindow::setAvailableRoomActions(const std::vector<RoomAction>& actions) {
	availableRoomActions_ = actions;
}

void QtChatWindow::setBlockingState(BlockingState state) {
	blockingState_ = state;
}

void QtChatWindow::setCanInitiateImpromptuChats(bool supportsImpromptu) {
	supportsImpromptuChat_ = supportsImpromptu;
}

void QtChatWindow::showRoomConfigurationForm(Form::ref form) {
	if (mucConfigurationWindow_) {
		delete mucConfigurationWindow_.data();
	}
	mucConfigurationWindow_ = new QtMUCConfigurationWindow(form);
	mucConfigurationWindow_->onFormComplete.connect(boost::bind(boost::ref(onConfigureRequest), _1));
	mucConfigurationWindow_->onFormCancelled.connect(boost::bind(boost::ref(onConfigurationFormCancelled)));
}

void QtChatWindow::handleAppendedToLog() {
	if (lastLineTracker_.getShouldMoveLastLine()) {
		/* should this be queued? */
		messageLog_->addLastSeenLine();
	}
	if (isWidgetSelected()) {
		onAllMessagesRead();
	}
}

void QtChatWindow::addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct, bool isImpromptu, bool isContinuation) {
	handleAppendedToLog();
	messageLog_->addMUCInvitation(senderName, jid, reason, password, direct, isImpromptu, isContinuation);
}

std::string QtChatWindow::addMessage(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	handleAppendedToLog();
	return messageLog_->addMessage(message, senderName, senderIsSelf, label, avatarPath, time, highlight);
}

std::string QtChatWindow::addAction(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	handleAppendedToLog();
	return messageLog_->addAction(message, senderName, senderIsSelf, label, avatarPath, time, highlight);
}


void QtChatWindow::addSystemMessage(const ChatMessage& message, Direction direction) {
	handleAppendedToLog();
	messageLog_->addSystemMessage(message, direction);
}

void QtChatWindow::addPresenceMessage(const ChatMessage& message, Direction direction) {
	handleAppendedToLog();
	messageLog_->addPresenceMessage(message, direction);
}

void QtChatWindow::addErrorMessage(const ChatMessage& message) {
	handleAppendedToLog();
	messageLog_->addErrorMessage(message);
}


void QtChatWindow::replaceMessage(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	handleAppendedToLog();
	messageLog_->replaceMessage(message, id, time, highlight);
}

void QtChatWindow::replaceWithAction(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	handleAppendedToLog();
	messageLog_->replaceWithAction(message, id, time, highlight);
}

std::string QtChatWindow::addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes) {
	handleAppendedToLog();
	return messageLog_->addFileTransfer(senderName, senderIsSelf, filename, sizeInBytes);
}

void QtChatWindow::setFileTransferProgress(std::string id, const int percentageDone) {
	messageLog_->setFileTransferProgress(id, percentageDone);
}

void QtChatWindow::setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg) {
	messageLog_->setFileTransferStatus(id, state, msg);
}


std::string QtChatWindow::addWhiteboardRequest(bool senderIsSelf) {
	handleAppendedToLog();
	return messageLog_->addWhiteboardRequest(contact_, senderIsSelf);
}

void QtChatWindow::setWhiteboardSessionStatus(std::string id, const ChatWindow::WhiteboardSessionState state) {
	messageLog_->setWhiteboardSessionStatus(id, state);
}

void QtChatWindow::replaceLastMessage(const ChatMessage& message) {
	messageLog_->replaceLastMessage(message);
}

void QtChatWindow::setAckState(const std::string& id, AckState state) {
	messageLog_->setAckState(id, state);
}

void QtChatWindow::setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) {
	messageLog_->setMessageReceiptState(id, state);
}

}
