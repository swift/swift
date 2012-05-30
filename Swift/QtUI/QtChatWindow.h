/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/QtUI/QtMUCConfigurationWindow.h>
#include <Swift/QtUI/QtAffiliationEditor.h>
#include <Swift/QtUI/QtSwiftUtil.h>

#include <QtTabbable.h>

#include <SwifTools/LastLineTracker.h>

#include <map>
#include <QPointer>
#include <QTextCursor>
#include <QMap>

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
				SecurityLabel::ref label = availableLabels_[index.row()].getLabel();
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
			static const QString ButtonWhiteboardSessionCancel;
			static const QString ButtonWhiteboardSessionAcceptRequest;
			static const QString ButtonWhiteboardShowWindow;
			static const QString ButtonFileTransferCancel;
			static const QString ButtonFileTransferSetDescription;
			static const QString ButtonFileTransferSendRequest;
			static const QString ButtonFileTransferAcceptRequest;
			static const QString ButtonMUCInvite;

		public:
			QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings, QMap<QString, QString> emoticons);
			~QtChatWindow();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			std::string addAction(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			void addSystemMessage(const std::string& message);
			void addPresenceMessage(const std::string& message);
			void addErrorMessage(const std::string& errorMessage);
			void replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time);
			void replaceWithAction(const std::string& message, const std::string& id, const boost::posix_time::ptime& time);
			// File transfer related stuff
			std::string addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes);
			void setFileTransferProgress(std::string id, const int percentageDone);
			void setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg);
			
			std::string addWhiteboardRequest(bool senderIsSelf);
			void setWhiteboardSessionStatus(std::string id, const ChatWindow::WhiteboardSessionState state);

			void show();
			void activate();
			void setUnreadMessageCount(int count);
			void convertToMUC();
//			TreeWidget *getTreeWidget();
			void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels);
			void setSecurityLabelsEnabled(bool enabled);
			void setSecurityLabelsError();
			SecurityLabelsCatalog::Item getSelectedSecurityLabel();
			void setName(const std::string& name);
			void setInputEnabled(bool enabled);
			QtTabbable::AlertType getWidgetAlertState();
			void setContactChatState(ChatState::ChatStateType state);
			void setRosterModel(Roster* roster);
			void setTabComplete(TabComplete* completer);
			int getCount();
			void replaceLastMessage(const std::string& message);
			void setAckState(const std::string& id, AckState state);

			// message receipts
			void setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state);

			void flash();
			QByteArray getSplitterState();
			virtual void setAvailableOccupantActions(const std::vector<OccupantAction>& actions);
			void setSubject(const std::string& subject);
			void showRoomConfigurationForm(Form::ref);
			void addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct = true);
			void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&);
			void setAvailableRoomActions(const std::vector<RoomAction> &actions);

			InviteToChatWindow* createInviteToChatWindow();

			static QString buildChatWindowButton(const QString& name, const QString& id, const QString& arg1 = QString(), const QString& arg2 = QString(), const QString& arg3 = QString());

		public slots:
			void handleChangeSplitterState(QByteArray state);
			void handleFontResized(int fontSizeSteps);
			void setAlert(const std::string& alertText, const std::string& buttonText = "");
			void cancelAlert();
			void setCorrectionEnabled(Tristate enabled);

		signals:
			void geometryChanged();
			void splitterMoved();
			void fontResized(int);

		protected slots:
			void qAppFocusChanged(QWidget* old, QWidget* now);
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

			void handleHTMLButtonClicked(QString id, QString arg1, QString arg2, QString arg3);
			void handleAffiliationEditorAccepted();
			void handleCurrentLabelChanged(int);

		private:
			enum PreviousMessageKind {
				PreviosuMessageWasNone,
				PreviousMessageWasMessage,
				PreviousMessageWasSystem,
				PreviousMessageWasPresence,
				PreviousMessageWasFileTransfer,
				PreviousMessageWasMUCInvite
			};

		private:
			void updateTitleWithUnreadCount();
			void tabComplete();
			void beginCorrection();
			void cancelCorrection();
			void handleSettingChanged(const std::string& setting);
			std::string addMessage(const QString& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time);
			void replaceMessage(const QString& message, const std::string& id, const boost::posix_time::ptime& time, const QString& style);
			void handleOccupantSelectionChanged(RosterItem* item);
			bool appendToPreviousCheck(PreviousMessageKind messageKind, const std::string& senderName, bool senderIsSelf) const;
			QString linkimoticonify(const QString& message) const;

			int unreadCount_;
			bool contactIsTyping_;
			LastLineTracker lastLineTracker_;
			QString contact_;
			QString lastSentMessage_;
			QTextCursor tabCompleteCursor_;
			QtChatView* messageLog_;
			QtChatTheme* theme_;
			QtTextEdit* input_;
			QWidget* midBar_; 
			QComboBox* labelsWidget_;
			QtOccupantListWidget* treeWidget_;
			QLabel* correctingLabel_;
			QLabel* alertLabel_;
			QWidget* alertWidget_;
			QPushButton* alertButton_;
			TabComplete* completer_;
			QLineEdit* subject_;
			QPushButton* actionButton_;
			bool isCorrection_;
			bool previousMessageWasSelf_;
			PreviousMessageKind previousMessageKind_;
			QString previousSenderName_;
			bool inputClearing_;
			bool tabCompletion_;
			UIEventStream* eventStream_;
			bool inputEnabled_;
			QSplitter *logRosterSplitter_;
			Tristate correctionEnabled_;
			QString alertStyleSheet_;
			std::map<QString, QString> descriptions;
			QtChatWindowJSBridge* jsBridge;
			QPointer<QtMUCConfigurationWindow> mucConfigurationWindow_;
			QPointer<QtAffiliationEditor> affiliationEditor_;
			int idCounter_;
			SettingsProvider* settings_;
			std::vector<ChatWindow::RoomAction> availableRoomActions_;
			QMap<QString, QString> emoticons_;
			bool showEmoticons_;
			QPalette defaultLabelsPalette_;
			LabelModel* labelModel_;
	};
}
