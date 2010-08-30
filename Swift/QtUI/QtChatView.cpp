/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatView.h"

#include <QtDebug>
#include <QFile>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QKeyEvent>
#include <QStackedWidget>

#include "QtWebView.h"
#include "QtChatTheme.h"


namespace Swift {

QtChatView::QtChatView(QtChatTheme* theme, QWidget* parent) : QWidget(parent) {
	theme_ = theme;

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);
	webView_ = new QtWebView(this);
	connect(webView_, SIGNAL(linkClicked(const QUrl&)), SLOT(handleLinkClicked(const QUrl&)));
	connect(webView_, SIGNAL(loadFinished(bool)), SLOT(handleViewLoadFinished(bool)));
	connect(webView_, SIGNAL(gotFocus()), SIGNAL(gotFocus()));
#ifdef Q_WS_X11
	/* To give a border on Linux, where it looks bad without */
	QStackedWidget* stack = new QStackedWidget(this);
	stack->addWidget(webView_);
	stack->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	stack->setLineWidth(2);
	mainLayout->addWidget(stack);
#else
	mainLayout->addWidget(webView_);
#endif


	

	webPage_ = new QWebPage(this);
	webPage_->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	webView_->setPage(webPage_);
	connect(webPage_, SIGNAL(selectionChanged()), SLOT(copySelectionToClipboard()));

	viewReady_ = false;
	QString pageHTML = theme_->getTemplate();
	pageHTML.replace("==bodyBackground==", "background-color:#e3e3e3");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, theme_->getBase());
	if (pageHTML.count("%@") > 3) {
		pageHTML.replace(pageHTML.indexOf("%@"), 2, theme_->getMainCSS());
	}
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "Variants/Blue on Green.css");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, ""/*headerSnippet.getContent()*/);
	pageHTML.replace(pageHTML.indexOf("%@"), 2, ""/*footerSnippet.getContent()*/);
	webPage_->mainFrame()->setHtml(pageHTML);
	document_ = webPage_->mainFrame()->documentElement();
	QWebElement chatElement = document_.findFirst("#Chat");
	newInsertPoint_ = chatElement.clone();
	newInsertPoint_.setOuterXml("<div id='swift_insert'/>");
	chatElement.appendInside(newInsertPoint_);
	Q_ASSERT(!newInsertPoint_.isNull());
}

void QtChatView::handleKeyPressEvent(QKeyEvent* event) {
	webView_->keyPressEvent(event);
}

void QtChatView::addMessage(boost::shared_ptr<ChatSnippet> snippet) {
	if (viewReady_) {
		addToDOM(snippet);
	} else {
		queuedSnippets_.append(snippet);
	}
}

QWebElement QtChatView::snippetToDOM(boost::shared_ptr<ChatSnippet> snippet) {
	QWebElement newElement = newInsertPoint_.clone();
	newElement.setInnerXml(snippet->getContent()); /* FIXME: Outer, surely? */
	Q_ASSERT(!newElement.isNull());
	return newElement;
}

void QtChatView::addToDOM(boost::shared_ptr<ChatSnippet> snippet) {
	QWebElement newElement = snippetToDOM(snippet);
	QWebElement continuationElement = lastElement_.findFirst("#insert");
	if (snippet->getAppendToPrevious()) {
		Q_ASSERT(!continuationElement.isNull());
		continuationElement.replace(newElement);
	} else {
		continuationElement.removeFromDocument();
		newInsertPoint_.prependOutside(newElement);
	}
	lastElement_ = newElement;
}

void QtChatView::correctLastMessage(const QString& newMessage) {
	/* FIXME: must be queued */
	lastElement_.findFirst("swift_message");
	lastElement_.setPlainText(ChatSnippet::escape(newMessage));
}

void QtChatView::correctLastMessage(const QString& newMessage, const QString& note) {
	correctLastMessage(newMessage);
	lastElement_.findFirst("swift_time");
	lastElement_.setPlainText(ChatSnippet::escape(note));
}

void QtChatView::copySelectionToClipboard() {
	if (!webPage_->selectedText().isEmpty()) {
		webPage_->triggerAction(QWebPage::Copy);
	}
}

bool QtChatView::isScrolledToBottom() const {
	return webPage_->mainFrame()->scrollBarValue(Qt::Vertical) == webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void QtChatView::scrollToBottom() {
	webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void QtChatView::handleLinkClicked(const QUrl& url) {
	QDesktopServices::openUrl(url);
}

void QtChatView::addQueuedSnippets() {
	for (int i = 0; i < queuedSnippets_.count(); i++) {
		addToDOM(queuedSnippets_[i]);
	}
	queuedSnippets_.clear();
}

void QtChatView::handleViewLoadFinished(bool ok) {
	Q_ASSERT(ok);
	viewReady_ = true;
	addQueuedSnippets();
//	webPage_->mainFrame()->evaluateJavaScript(queuedMessages_);
//	queuedMessages_.clear();
}

}
