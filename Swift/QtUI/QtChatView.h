/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtChatView_H
#define SWIFT_QtChatView_H

#include <QString>
#include <QWidget>
#include <QList>
#include <QWebElement>

#include <boost/shared_ptr.hpp>

#include "ChatSnippet.h"

class QWebPage;
class QUrl;

namespace Swift {
	class QtWebView;
	class QtChatTheme;
	class QtChatView : public QWidget {
			Q_OBJECT
		public:
			QtChatView(QtChatTheme* theme, QWidget* parent);

			void addMessage(boost::shared_ptr<ChatSnippet> snippet);
			void correctLastMessage(const QString& newMessage);
			void correctLastMessage(const QString& newMessage, const QString& note);
			bool isScrolledToBottom() const;

		signals:
			void gotFocus();

		public slots:
			void copySelectionToClipboard();
			void scrollToBottom();
			void handleLinkClicked(const QUrl&);
			void handleKeyPressEvent(QKeyEvent* event);

		private slots:
			void handleViewLoadFinished(bool);

		private:
			void headerEncode();
			void messageEncode();
			void addQueuedSnippets();
			void addToDOM(boost::shared_ptr<ChatSnippet> snippet);
			QWebElement snippetToDOM(boost::shared_ptr<ChatSnippet> snippet);

			bool viewReady_;
			QtWebView* webView_;
			QWebPage* webPage_;
			QList<boost::shared_ptr<ChatSnippet> > queuedSnippets_;

			QtChatTheme* theme_;
			QWebElement newInsertPoint_;
			QWebElement lastElement_;
			QWebElement document_;
	};
}

#endif
