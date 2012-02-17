/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/QtUI/QtMUCConfigurationWindow.h>
#include <Swift/QtUI/QtAffiliationEditor.h>

#include <QtTabbable.h>

#include <SwifTools/LastLineTracker.h>

#include <map>
#include <QPointer>

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
	class QtFileTransferJSBridge;
	class SettingsProvider;

	class QtChatWindow : public QtTabbable, public ChatWindow {
		Q_OBJECT
		public:
			QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream, SettingsProvider* settings);
			~QtChatWindow();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			std::string addAction(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			void addSystemMessage(const std::string& message);
			void addPresenceMessage(const std::string& message);
			void addErrorMessage(const std::string& errorMessage);
			void replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time);
			// File transfer related stuff
			std::string addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes);
			void setFileTransferProgress(std::string id, const int percentageDone);
			void setFileTransferStatus(std::string id, const FileTransferState state, const std::string& msg);
			
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
			void addMUCInvitation(const JID& jid, const std::string& reason, const std::string& password, bool direct = true);
			void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&);

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
			void returnPressed();
			void handleInputChanged();
			void handleKeyPressEvent(QKeyEvent* event);
			void handleSplitterMoved(int pos, int index);
			void handleAlertButtonClicked();
			void handleActionButtonClicked();

			
			void handleFileTransferCancel(QString id);
			void handleFileTransferSetDescription(QString id);
			void handleFileTransferStart(QString id);
			void handleFileTransferAccept(QString id, QString filename);
			void handleAffiliationEditorAccepted();

		private:
			void updateTitleWithUnreadCount();
			void tabComplete();
			void beginCorrection();
			void cancelCorrection();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time);
			void handleOccupantSelectionChanged(RosterItem* item);

			int unreadCount_;
			bool contactIsTyping_;
			LastLineTracker lastLineTracker_;
			QString contact_;
			QString lastSentMessage_;
			QtChatView* messageLog_;
			QtChatTheme* theme_;
			QtTextEdit* input_;
			QComboBox* labelsWidget_;
			QtOccupantListWidget* treeWidget_;
			QLabel* correctingLabel_;
			QLabel* alertLabel_;
			QWidget* alertWidget_;
			QPushButton* alertButton_;
			TabComplete* completer_;
			QLineEdit* subject_;
			QPushButton* actionButton_;
			std::vector<SecurityLabelsCatalog::Item> availableLabels_;
			bool isCorrection_;
			bool previousMessageWasSelf_;
			bool previousMessageWasSystem_;
			bool previousMessageWasPresence_;
			bool previousMessageWasFileTransfer_;
			QString previousSenderName_;
			bool inputClearing_;
			UIEventStream* eventStream_;
			bool inputEnabled_;
			QSplitter *logRosterSplitter_;
			Tristate correctionEnabled_;
			QString alertStyleSheet_;
			std::map<QString, QString> descriptions;
			QtFileTransferJSBridge* fileTransferJS;
			QPointer<QtMUCConfigurationWindow> mucConfigurationWindow_;
			QPointer<QtAffiliationEditor> affiliationEditor_;
			int idCounter_;
			SettingsProvider* settings_;
	};
}
