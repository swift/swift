/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/ChatWindow.h"

#include "QtTabbable.h"

#include "SwifTools/LastLineTracker.h"

#include "Swiften/Base/IDGenerator.h"

class QTextEdit;
class QLineEdit;
class QComboBox;
class QLabel;
class QSplitter;

namespace Swift {
	class QtChatView;
	class QtTreeWidget;
	class QtTreeWidgetFactory;
	class QtChatTheme;
	class TreeWidget;
	class QtTextEdit;
	class UIEventStream;
	class QtChatWindow : public QtTabbable, public ChatWindow {
		Q_OBJECT
		public:
			QtChatWindow(const QString &contact, QtChatTheme* theme, UIEventStream* eventStream);
			~QtChatWindow();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			std::string addAction(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			void addSystemMessage(const std::string& message);
			void addPresenceMessage(const std::string& message);
			void addErrorMessage(const std::string& errorMessage);
			void replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time);
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
			void flash();
			QByteArray getSplitterState();

			void setJID(const JID& jid) {}

		public slots:
			void handleChangeSplitterState(QByteArray state);
			void handleFontResized(int fontSizeSteps);

		signals:
			void geometryChanged();
			void splitterMoved();
			void fontResized(int);

		protected slots:
			void qAppFocusChanged(QWidget* old, QWidget* now);
			void closeEvent(QCloseEvent* event);
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);

		protected:
			void showEvent(QShowEvent* event);

		private slots:
			void returnPressed();
			void handleInputChanged();
			void handleKeyPressEvent(QKeyEvent* event);
			void handleSplitterMoved(int pos, int index);

		private:
			void updateTitleWithUnreadCount();
			void tabComplete();
			void beginCorrection();
			void cancelCorrection();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time);

			int unreadCount_;
			bool contactIsTyping_;
			LastLineTracker lastLineTracker_;
			QString contact_;
			QString lastSentMessage_;
			QtChatView* messageLog_;
			QtChatTheme* theme_;
			QtTextEdit* input_;
			QComboBox* labelsWidget_;
			QtTreeWidget* treeWidget_;
			QLabel* correctingLabel_;
			TabComplete* completer_;
			std::vector<SecurityLabelsCatalog::Item> availableLabels_;
			bool isCorrection_;
			bool previousMessageWasSelf_;
			bool previousMessageWasSystem_;
			bool previousMessageWasPresence_;
			QString previousSenderName_;
			bool inputClearing_;
			UIEventStream* eventStream_;
			bool inputEnabled_;
			IDGenerator id_;
			QSplitter *logRosterSplitter_;
	};
}
