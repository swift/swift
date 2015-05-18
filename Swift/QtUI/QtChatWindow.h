/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <QMap>
#include <QMenu>
#include <QPointer>
#include <QString>
#include <QTextCursor>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <SwifTools/LastLineTracker.h>

#include <Swift/QtUI/ChatSnippet.h>
#include <Swift/QtUI/QtAffiliationEditor.h>
#include <Swift/QtUI/QtEmoticonsGrid.h>
#include <Swift/QtUI/QtMUCConfigurationWindow.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtTabbable.h>

class QTextEdit;
class QLineEdit;
class QComboBox;
class QLabel;
class QSplitter;
class QPushButton;

namespace Swift {
	class QtChatView;
	class QtOccupantListWidget;
	class QtChatTheme;
	class TreeWidget;
	class QtTextEdit;
	class UIEventStream;
	class QtChatWindowJSBridge;
	class SettingsProvider;

	class LabelModel : public QAbstractListModel {
		Q_OBJECT
		public:
			LabelModel(QObject* parent = NULL) : QAbstractListModel(parent) {}

			virtual int rowCount(const QModelIndex& /*index*/) const {
				return static_cast<int>(availableLabels_.size());
			}

			virtual QVariant data(const QModelIndex& index, int role) const {
				if (!index.isValid()) {
					return QVariant();
				}
				boost::shared_ptr<SecurityLabel> label = availableLabels_[index.row()].getLabel();
				if (label && role == Qt::TextColorRole) {
					return P2QSTRING(label->getForegroundColor());
				}
				if (label && role == Qt::TextColorRole) {
					return P2QSTRING(label->getBackgroundColor());
				}
				if (role == Qt::DisplayRole) {
					std::string selector = availableLabels_[index.row()].getSelector();
					std::string displayMarking = label ? label->getDisplayMarking() : "";
					QString labelName = selector.empty() ? displayMarking.c_str() : selector.c_str();
					return labelName;
				}
				return QVariant();
			}

			std::vector<SecurityLabelsCatalog::Item> availableLabels_;
	};

	class QtChatWindow : public QtTabbable, public ChatWindow {
		Q_OBJECT

		public:
			QtChatWindow(const QString& contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings, const std::map<std::string, std::string>& emoticons);
			virtual ~QtChatWindow();
			std::string addMessage(const ChatMessage& message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			std::string addAction(const ChatMessage& message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight);

			std::string addSystemMessage(const ChatMessage& message, Direction direction);
			void addPresenceMessage(const ChatMessage& message, Direction direction);
			void addErrorMessage(const ChatMessage& message);

			void replaceMessage(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			void replaceWithAction(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			// File transfer related stuff
			std::string addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes);
			void setFileTransferProgress(std::string id, const int percentageDone);
			void setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg);
			
			std::string addWhiteboardRequest(bool senderIsSelf);
			void setWhiteboardSessionStatus(std::string id, const ChatWindow::WhiteboardSessionState state);

			void show();
			bool isVisible() const;
			void activate();
			void setUnreadMessageCount(int count);
			void convertToMUC(MUCType mucType);
//			TreeWidget *getTreeWidget();
			void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels);
			void setSecurityLabelsEnabled(bool enabled);
			void setSecurityLabelsError();
			SecurityLabelsCatalog::Item getSelectedSecurityLabel();
			void setName(const std::string& name);
			void setOnline(bool online);
			QtTabbable::AlertType getWidgetAlertState();
			void setContactChatState(ChatState::ChatStateType state);
			void setRosterModel(Roster* roster);
			void setTabComplete(TabComplete* completer);
			int getCount();
			virtual void replaceSystemMessage(const ChatMessage& message, const std::string& id, const TimestampBehaviour timestampBehaviour);
			void replaceLastMessage(const ChatMessage& message, const TimestampBehaviour timestampBehaviour);
			void setAckState(const std::string& id, AckState state);

			// message receipts
			void setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state);

			void flash();
			QByteArray getSplitterState();
			virtual void setAvailableOccupantActions(const std::vector<OccupantAction>& actions);
			void setSubject(const std::string& subject);
			void showRoomConfigurationForm(Form::ref);
			void addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct = true, bool isImpromptu = false, bool isContinuation = false);
			void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&);
			void setAvailableRoomActions(const std::vector<RoomAction>& actions);
			void setBlockingState(BlockingState state);
			virtual void setCanInitiateImpromptuChats(bool supportsImpromptu);
			virtual void showBookmarkWindow(const MUCBookmark& bookmark);
			virtual void setBookmarkState(RoomBookmarkState bookmarkState);
			virtual std::string getID() const;

		public slots:
			void handleChangeSplitterState(QByteArray state);
			void handleFontResized(int fontSizeSteps);
			AlertID addAlert(const std::string& alertText);
			void removeAlert(const AlertID id);
			void setCorrectionEnabled(Tristate enabled);
			void setFileTransferEnabled(Tristate enabled);

		signals:
			void geometryChanged();
			void splitterMoved();
			void fontResized(int);

		protected slots:
			void closeEvent(QCloseEvent* event);
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);

			void dragEnterEvent(QDragEnterEvent *event);
			void dropEvent(QDropEvent *event);

		protected:
			void showEvent(QShowEvent* event);

		private slots:
			void handleLogCleared();
			void returnPressed();
			void handleInputChanged();
			void handleCursorPositionChanged();
			void handleKeyPressEvent(QKeyEvent* event);
			void handleSplitterMoved(int pos, int index);
			void handleAlertButtonClicked();
			void handleActionButtonClicked();
			void handleAffiliationEditorAccepted();
			void handleCurrentLabelChanged(int);
			void handleEmoticonsButtonClicked();
			void handleEmoticonClicked(QString emoticonAsText);
			void handleTextInputReceivedFocus();
			void handleTextInputLostFocus();

		private:
			void updateTitleWithUnreadCount();
			void tabComplete();
			void beginCorrection();
			void cancelCorrection();
			void handleSettingChanged(const std::string& setting);

			void handleOccupantSelectionChanged(RosterItem* item);
			void handleAppendedToLog();

		private:
			int unreadCount_;
			bool contactIsTyping_;
			LastLineTracker lastLineTracker_;
			std::string id_;
			QString contact_;
			QString lastSentMessage_;
			QTextCursor tabCompleteCursor_;
			QtChatView* messageLog_;
			QtChatTheme* theme_;
			QtTextEdit* input_;
			QWidget* midBar_; 
			QBoxLayout* subjectLayout_;
			QComboBox* labelsWidget_;
			QtOccupantListWidget* treeWidget_;
			QLabel* correctingLabel_;
			boost::optional<AlertID> correctingAlert_;
			QVBoxLayout* alertLayout_;
			std::map<AlertID, QWidget*> alertWidgets_;
			AlertID nextAlertId_;
			TabComplete* completer_;
			QLineEdit* subject_;
			bool isCorrection_;
			bool inputClearing_;
			bool tabCompletion_;
			UIEventStream* eventStream_;
			bool isOnline_;
			QSplitter *logRosterSplitter_;
			Tristate correctionEnabled_;
			Tristate fileTransferEnabled_;
			QString alertStyleSheet_;
			QPointer<QtMUCConfigurationWindow> mucConfigurationWindow_;
			QPointer<QtAffiliationEditor> affiliationEditor_;
			SettingsProvider* settings_;
			std::vector<ChatWindow::RoomAction> availableRoomActions_;
			QPalette defaultLabelsPalette_;
			LabelModel* labelModel_;
			BlockingState blockingState_;
			bool impromptu_;
			bool isMUC_;
			bool supportsImpromptuChat_;
			RoomBookmarkState roomBookmarkState_;
			QMenu* emoticonsMenu_;
	};
}
