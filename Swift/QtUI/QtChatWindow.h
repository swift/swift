#ifndef SWIFT_QtChatWindow_H
#define SWIFT_QtChatWindow_H

#include "Swift/Controllers/ChatWindow.h"

#include "QtTabbable.h"

class QTextEdit;
class QLineEdit;
class QComboBox;

namespace Swift {
	class QtChatView;
	class QtTreeWidget;
	class QtTreeWidgetFactory;
	class TreeWidget;
	class QtTextEdit;
	class QtChatWindow : public QtTabbable, public ChatWindow {
		Q_OBJECT
		public:
			QtChatWindow(const QString &contact, QtTreeWidgetFactory* treeWidgetFactory);
			void addMessage(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath);
			void addSystemMessage(const String& message);
			void addErrorMessage(const String& errorMessage);
			void show();
			void setUnreadMessageCount(int count);
			void convertToMUC();
			TreeWidget *getTreeWidget();
			void setAvailableSecurityLabels(const std::vector<SecurityLabel>& labels);
			void setSecurityLabelsEnabled(bool enabled);
			void setSecurityLabelsError();
			SecurityLabel getSelectedSecurityLabel();
			void setName(const String& name);

		protected slots:
			void qAppFocusChanged(QWidget* old, QWidget* now);
			void closeEvent(QCloseEvent* event);

		protected:
			void showEvent(QShowEvent* event);
		private slots:
			void returnPressed();

		private:
			void updateTitleWithUnreadCount();

			int unreadCount_;
			QString contact_;
			QtChatView *messageLog_;
			QtTextEdit* input_;
			QComboBox *labelsWidget_;
			QtTreeWidget *treeWidget_;
			std::vector<SecurityLabel> availableLabels_;
			bool previousMessageWasSelf_;
			bool previousMessageWasSystem_;
			QString previousSenderName_;
	};
}

#endif
