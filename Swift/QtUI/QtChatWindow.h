/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtChatWindow_H
#define SWIFT_QtChatWindow_H

#include "Swift/Controllers/UIInterfaces/ChatWindow.h"

#include "QtTabbable.h"

#include "Swiften/Base/IDGenerator.h"

class QTextEdit;
class QLineEdit;
class QComboBox;

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

		signals:
			void geometryChanged();

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

		private:
			void updateTitleWithUnreadCount();
			void tabComplete();
			std::string addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const QString& style, const boost::posix_time::ptime& time);

			int unreadCount_;
			bool contactIsTyping_;
			QString contact_;
			QtChatView* messageLog_;
			QtChatTheme* theme_;
			QtTextEdit* input_;
			QComboBox* labelsWidget_;
			QtTreeWidget* treeWidget_;
			TabComplete* completer_;
			std::vector<SecurityLabelsCatalog::Item> availableLabels_;
			bool previousMessageWasSelf_;
			bool previousMessageWasSystem_;
			bool previousMessageWasPresence_;
			QString previousSenderName_;
			bool inputClearing_;
			UIEventStream* eventStream_;
			bool inputEnabled_;
			IDGenerator id_;
	};
}

#endif
