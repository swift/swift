/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtChatView_H
#define SWIFT_QtChatView_H

#include <QString>
#include <QWidget>

#include "ChatSnippet.h"

class QWebPage;
class QUrl;

namespace Swift {
	class QtWebView;
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
			void handleKeyPressEvent(QKeyEvent* event);

		private slots:
			void handleViewLoadFinished(bool);

		private:
			bool viewReady_;
			QtWebView* webView_;
			QWebPage* webPage_;
			QString previousContinuationElementID_;
			QString queuedMessages_;
	};
}

#endif
