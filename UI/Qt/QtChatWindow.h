#ifndef SWIFT_QtChatWindow_H
#define SWIFT_QtChatWindow_H

#include "Swiften/Controllers/ChatWindow.h"

#include <QWidget>

class QTextEdit;
class QLineEdit;
class QComboBox;

namespace Swift {
	class QtChatView;
	class QtTreeWidget;
	class QtTreeWidgetFactory;
	class TreeWidget;
	class QtChatWindow : public QWidget, public ChatWindow {
		Q_OBJECT
		public:
			QtChatWindow(const QString &contact, QtTreeWidgetFactory* treeWidgetFactory);
			void addMessage(const String &message, const String &senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label);
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

		protected slots:
			void qAppFocusChanged(QWidget* old, QWidget* now);
			void closeEvent(QCloseEvent* event);

		private slots:
			void returnPressed();

		private:
			void updateTitleWithUnreadCount();

			int unreadCount_;
			QString contact_;
			QtChatView *messageLog_;
			QLineEdit* input_;
			QComboBox *labelsWidget_;
			QtTreeWidget *treeWidget_;
			std::vector<SecurityLabel> availableLabels_;
			bool previousMessageWasSelf_;
			bool previousMessageWasSystem_;
			QString previousSenderName_;
	};
}

#endif
