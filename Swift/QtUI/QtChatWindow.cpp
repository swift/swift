/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatWindow.h>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QComboBox>
#include <QCursor>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPoint>
#include <QPushButton>
#include <QSize>
#include <QSplitter>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <QTime>
#include <QToolButton>
#include <QUrl>

#include <Swiften/Base/Log.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/RosterItem.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <SwifTools/TabComplete.h>

#include <Swift/QtUI/QtAddBookmarkWindow.h>
#include <Swift/QtUI/QtEditBookmarkWindow.h>
#include <Swift/QtUI/QtPlainChatView.h>
#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtTextEdit.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/QtWebKitChatView.h>
#include <Swift/QtUI/Roster/QtOccupantListWidget.h>

namespace Swift {

QtChatWindow::QtChatWindow(const QString& contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings, const std::map<std::string, std::string>& emoticons) : QtTabbable(), id_(Q2PSTRING(contact)), contact_(contact), nextAlertId_(0), eventStream_(eventStream), blockingState_(BlockingUnsupported), isMUC_(false), supportsImpromptuChat_(false), roomBookmarkState_(RoomNotBookmarked) {
	settings_ = settings;
	unreadCount_ = 0;
	isOnline_ = true;
	completer_ = NULL;
	affiliationEditor_ = NULL;
	theme_ = theme;
	isCorrection_ = false;
	labelModel_ = NULL;
	correctionEnabled_ = Maybe;
	fileTransferEnabled_ = Maybe;
	updateTitleWithUnreadCount();
	assert(settings);
#ifdef SWIFT_EXPERIMENTAL_FT
	setAcceptDrops(true);
#endif

	alertStyleSheet_ = "background: rgb(255, 255, 153); color: black";

	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);

	alertLayout_ = new QVBoxLayout();
	layout->addLayout(alertLayout_);

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
	if (settings_->getSetting(QtUISettingConstants::USE_PLAIN_CHATS) || settings_->getSetting(QtUISettingConstants::USE_SCREENREADER)) {
		messageLog_ = new QtPlainChatView(this, eventStream_);
	}
	else {
		messageLog_ = new QtWebKitChatView(this, eventStream_, theme, this); // I accept that passing the ChatWindow in so that the view can call the signals is somewhat inelegant, but it saves a lot of boilerplate. This patch is unpleasant enough already. So let's fix this soon (it at least needs fixing by the time history is sorted), but not now.
	}
	logRosterSplitter_->addWidget(messageLog_);

	treeWidget_ = new QtOccupantListWidget(eventStream_, settings_, QtTreeWidget::MessageDefaultJID, this);
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

	connect(input_, SIGNAL(receivedFocus()), this, SLOT(handleTextInputReceivedFocus()));
	connect(input_, SIGNAL(lostFocus()), this, SLOT(handleTextInputLostFocus()));
	QPushButton* emoticonsButton_ = new QPushButton(this);
	emoticonsButton_->setIcon(QIcon(":/emoticons/smile.png"));
	connect(emoticonsButton_, SIGNAL(clicked()), this, SLOT(handleEmoticonsButtonClicked()));

	emoticonsMenu_ = new QMenu(this);
	QtEmoticonsGrid* emoticonsGrid = new QtEmoticonsGrid(emoticons, emoticonsMenu_);
	connect(emoticonsGrid, SIGNAL(emoticonClicked(QString)), this, SLOT(handleEmoticonClicked(QString)));

	// using an extra layout to work around Qt margin glitches on OS X
	QHBoxLayout* actionLayout = new QHBoxLayout();
	actionLayout->addWidget(emoticonsButton_);
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
	connect(messageLog_, SIGNAL(gotFocus()), input_, SLOT(setFocus()));
	resize(400,300);
	connect(messageLog_, SIGNAL(fontResized(int)), this, SIGNAL(fontResized(int)));
	connect(messageLog_, SIGNAL(logCleared()), this, SLOT(handleLogCleared()));

	treeWidget_->onSomethingSelectedChanged.connect(boost::bind(&QtChatWindow::handleOccupantSelectionChanged, this, _1));
	treeWidget_->onOccupantActionSelected.connect(boost::bind(boost::ref(onOccupantActionSelected), _1, _2));

	settings_->onSettingChanged.connect(boost::bind(&QtChatWindow::handleSettingChanged, this, _1));
	messageLog_->showEmoticons(settings_->getSetting(QtUISettingConstants::SHOW_EMOTICONS));
	setMinimumSize(100, 100);
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
	const QObject* alertWidget = QObject::sender()->parent();
	std::map<AlertID, QWidget*>::const_iterator i = alertWidgets_.begin();
	for ( ; i != alertWidgets_.end(); ++i) {
		if (i->second == alertWidget) {
			removeAlert(i->first);
			break;
		}
	}
}

QtChatWindow::AlertID QtChatWindow::addAlert(const std::string& alertText) {
	QWidget* alertWidget = new QWidget(this);
	QHBoxLayout* alertLayout = new QHBoxLayout(alertWidget);
	alertLayout_->addWidget(alertWidget);
	QLabel* alertLabel = new QLabel(this);
	alertLabel->setText(alertText.c_str());
	alertLayout->addWidget(alertLabel);

	QToolButton* closeButton = new QToolButton(alertWidget);
	closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	closeButton->setIconSize(QSize(16,16));
	closeButton->setCursor(Qt::ArrowCursor);
	closeButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	connect (closeButton, SIGNAL(clicked()), this, SLOT(handleAlertButtonClicked()));

	alertLayout->addWidget(closeButton);
	QPalette palette = alertWidget->palette();
	palette.setColor(QPalette::Window, QColor(Qt::yellow));
	palette.setColor(QPalette::WindowText, QColor(Qt::black));
	alertWidget->setStyleSheet(alertStyleSheet_);
	alertLabel->setStyleSheet(alertStyleSheet_);

	AlertID id = nextAlertId_++;
	alertWidgets_[id] = alertWidget;
	return id;
}

void QtChatWindow::removeAlert(const AlertID id) {
	std::map<AlertID, QWidget*>::iterator i = alertWidgets_.find(id);
	if (i != alertWidgets_.end()) {
		alertLayout_->removeWidget(i->second);
		delete i->second;
		alertWidgets_.erase(i);
	}
}

void QtChatWindow::setTabComplete(TabComplete* completer) {
	completer_ = completer;
}

void QtChatWindow::handleKeyPressEvent(QKeyEvent* event) {
	event->ignore();
	if (event->isAccepted()) {
		return;
	}
	event->accept();

	int key = event->key();
	if (key == Qt::Key_Tab) {
		tabComplete();
	}
	else if ((key == Qt::Key_Up) && input_->toPlainText().isEmpty() && !(lastSentMessage_.isEmpty())) {
		beginCorrection();
	}
	else if (key == Qt::Key_Down && isCorrection_ && input_->textCursor().atBlockEnd()) {
		cancelCorrection();
	}
	else if (key == Qt::Key_Down || key == Qt::Key_Up) {
		event->ignore();
	}
	else {
		messageLog_->handleKeyPressEvent(event);
	}
}

void QtChatWindow::beginCorrection() {
	boost::optional<AlertID> newCorrectingAlert;
	if (correctionEnabled_ == Maybe) {
		newCorrectingAlert = addAlert(Q2PSTRING(tr("This chat may not support message correction. If you send a correction anyway, it may appear as a duplicate message")));
	}
	else if (correctionEnabled_ == No) {
		newCorrectingAlert = addAlert(Q2PSTRING(tr("This chat does not support message correction.  If you send a correction anyway, it will appear as a duplicate message")));
	}

	if (newCorrectingAlert) {
		if (correctingAlert_) {
			removeAlert(*correctingAlert_);
		}
		correctingAlert_ = newCorrectingAlert;
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
	if (correctingAlert_) {
		removeAlert(*correctingAlert_);
		correctingAlert_.reset();
	}
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
	}
	else {
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
		SWIFT_LOG(debug) << "User selected a label that doesn't exist";
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
	}
	else {
		labelsWidget_->hide();
	}
}

void QtChatWindow::setCorrectionEnabled(Tristate enabled) {
	correctionEnabled_ = enabled;
}

void QtChatWindow::setFileTransferEnabled(Tristate enabled) {
	fileTransferEnabled_ = enabled;
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

void QtChatWindow::convertToMUC(MUCType mucType) {
	impromptu_ = (mucType == ImpromptuMUC);
	treeWidget_->setMessageTarget(impromptu_ ? QtTreeWidget::MessageDisplayJID : QtTreeWidget::MessageDefaultJID);
	isMUC_ = true;
	treeWidget_->show();
	subject_->setVisible(!impromptu_);
}

void QtChatWindow::setOnline(bool online) {
	isOnline_ = online;
	if (!online) {
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
	}
	else {
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

void QtChatWindow::replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& id, const ChatWindow::TimestampBehaviour timestampBehaviour) {
	messageLog_->replaceSystemMessage(message, id, timestampBehaviour);
}

void QtChatWindow::returnPressed() {
	if (!isOnline_ || (blockingState_ == IsBlocked)) {
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
	}
	else {
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

bool QtChatWindow::isVisible() const {
	return QWidget::isVisible();
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
	if (isOnline_ && (blockingState_ != IsBlocked)) {
		if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
			// TODO: check whether contact actually supports file transfer
			if (!isMUC_) {
				event->acceptProposedAction();
			}
		}
		else if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid-list")) {
			if (isMUC_ || supportsImpromptuChat_) {
				event->acceptProposedAction();
			}
		}
	}
}

void QtChatWindow::dropEvent(QDropEvent *event) {
	if (fileTransferEnabled_ == Yes && event->mimeData()->hasUrls()) {
		if (event->mimeData()->urls().size() == 1) {
			onSendFileRequest(Q2PSTRING(event->mimeData()->urls().at(0).toLocalFile()));
		}
		else {
			std::string messageText(Q2PSTRING(tr("Sending of multiple files at once isn't supported at this time.")));
			ChatMessage message;
			message.append(boost::make_shared<ChatTextMessagePart>(messageText));
			addSystemMessage(message, DefaultDirection);
		}
	}
	else if (event->mimeData()->hasFormat("application/vnd.swift.contact-jid-list")) {
		QByteArray dataBytes = event->mimeData()->data("application/vnd.swift.contact-jid-list");
		QDataStream dataStream(&dataBytes, QIODevice::ReadOnly);
		std::vector<JID> invites;
		while (!dataStream.atEnd()) {
			QString jidString;
			dataStream >> jidString;
			invites.push_back(Q2PSTRING(jidString));
		}
		onInviteToChat(invites);
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

void QtChatWindow::handleEmoticonsButtonClicked() {
	emoticonsMenu_->adjustSize();
	QSize menuSize = emoticonsMenu_->size();
	emoticonsMenu_->exec(QPoint(QCursor::pos().x() - menuSize.width(), QCursor::pos().y() - menuSize.height()));
}

void QtChatWindow::handleEmoticonClicked(QString emoticonAsText) {
	input_->textCursor().insertText(emoticonAsText);
}

void QtChatWindow::handleTextInputReceivedFocus() {
	lastLineTracker_.setHasFocus(true);
	input_->setFocus();
	onAllMessagesRead();
}

void QtChatWindow::handleTextInputLostFocus() {
	lastLineTracker_.setHasFocus(false);
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
			unblock->setEnabled(isOnline_);
		}
		else if (!isMUC_ && blockingState_ == IsUnblocked) {
			block = contextMenu.addAction(tr("Block"));
			block->setEnabled(isOnline_);
		}

		if (supportsImpromptuChat_) {
			invite = contextMenu.addAction(tr("Invite person to this chat…"));
			invite->setEnabled(isOnline_ && (blockingState_ != IsBlocked));
		}

	}
	else {
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
				case ChatWindow::ChangeSubject:
					changeSubject = contextMenu.addAction(tr("Change subject…"));
					changeSubject->setEnabled(isOnline_);
					break;
				case ChatWindow::Configure:
					configure = contextMenu.addAction(tr("Configure room…"));
					configure->setEnabled(isOnline_);
					break;
				case ChatWindow::Affiliations:
					affiliations = contextMenu.addAction(tr("Edit affiliations…"));
					affiliations->setEnabled(isOnline_);
					break;
				case ChatWindow::Destroy:
					destroy = contextMenu.addAction(tr("Destroy room"));
					destroy->setEnabled(isOnline_);
					break;
				case ChatWindow::Invite:
					invite = contextMenu.addAction(tr("Invite person to this room…"));
					invite->setEnabled(isOnline_);
					break;
			}
		}
	}

	QAction* bookmark = NULL;
	if (isMUC_) {
		if (roomBookmarkState_ == RoomNotBookmarked) {
			bookmark = contextMenu.addAction(tr("Bookmark this room..."));
		}
		else {
			bookmark = contextMenu.addAction(tr("Edit bookmark..."));
		}
		bookmark->setEnabled(isOnline_);
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
	else if (result == bookmark) {
		onBookmarkRequest();
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

void QtChatWindow::showBookmarkWindow(const MUCBookmark& bookmark) {
	if (roomBookmarkState_ == RoomNotBookmarked) {
		QtAddBookmarkWindow* window = new QtAddBookmarkWindow(eventStream_, bookmark);
		window->show();
	}
	else {
		QtEditBookmarkWindow* window = new QtEditBookmarkWindow(eventStream_, bookmark);
		window->show();
	}
}

std::string QtChatWindow::getID() const {
	return id_;
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


std::string QtChatWindow::addSystemMessage(const ChatMessage& message, Direction direction) {
	handleAppendedToLog();
	return messageLog_->addSystemMessage(message, direction);
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

void QtChatWindow::replaceLastMessage(const ChatMessage& message, const TimestampBehaviour timestampBehaviour) {
	messageLog_->replaceLastMessage(message, timestampBehaviour);
}

void QtChatWindow::setAckState(const std::string& id, AckState state) {
	messageLog_->setAckState(id, state);
}

void QtChatWindow::setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) {
	messageLog_->setMessageReceiptState(id, state);
}

void QtChatWindow::setBookmarkState(RoomBookmarkState bookmarkState) {
	roomBookmarkState_ = bookmarkState;
}

}
