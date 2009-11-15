#ifndef SWIFT_QtChatView_H
#define SWIFT_QtChatView_H

#include <QString>
#include <QWidget>

#include "ChatSnippet.h"

class QWebView;
class QWebPage;
class QUrl;

namespace Swift {
	class QtChatView : public QWidget {
			Q_OBJECT
		public:
			QtChatView(QWidget* parent);

			void addMessage(const ChatSnippet& snippet);
			bool isScrolledToBottom() const;

		public slots:
			void copySelectionToClipboard();
			void scrollToBottom();
			void handleLinkClicked(const QUrl&);

		private:
			QWebView* webView_;
			QWebPage* webPage_;
			QString previousContinuationElementID_;
	};
}

#endif
