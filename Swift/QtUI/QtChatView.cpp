/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtChatView.h"

#include <QtDebug>
#include <QEventLoop>
#include <QFile>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QKeyEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QMessageBox>
#include <QApplication>

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
	connect(webView_, SIGNAL(clearRequested()), SLOT(handleClearRequested()));
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
	isAtBottom_ = true;
	resetView();
}

void QtChatView::handleClearRequested() {
	QMessageBox messageBox(this);
	messageBox.setWindowTitle(tr("Clear log"));
	messageBox.setText(tr("You are about to clear the contents of your chat log."));
	messageBox.setInformativeText(tr("Are you sure?"));
	messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	messageBox.setDefaultButton(QMessageBox::Yes);
	int button = messageBox.exec();
	if (button == QMessageBox::Yes) {
		resetView();
	}
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
	rememberScrolledToBottom();
	bool insert = snippet->getAppendToPrevious();
	QWebElement continuationElement = lastElement_.findFirst("#insert");
	bool fallback = insert && continuationElement.isNull();
	boost::shared_ptr<ChatSnippet> newSnippet = (insert && fallback) ? snippet->getContinuationFallbackSnippet() : snippet;
	QWebElement newElement = snippetToDOM(newSnippet);
	if (insert && !fallback) {
		Q_ASSERT(!continuationElement.isNull());
		continuationElement.replace(newElement);
	} else {
		continuationElement.removeFromDocument();
		newInsertPoint_.prependOutside(newElement);
	}
	lastElement_ = newElement;
	//qApp->processEvents();
}

void QtChatView::replaceLastMessage(const QString& newMessage) {
	assert(viewReady_);
	/* FIXME: must be queued? */
	rememberScrolledToBottom();
	assert(!lastElement_.isNull());
	QWebElement replace = lastElement_.findFirst("span.swift_message");
	assert(!replace.isNull());
	QString old = lastElement_.toOuterXml();
	replace.setInnerXml(ChatSnippet::escape(newMessage));
}

void QtChatView::replaceLastMessage(const QString& newMessage, const QString& note) {
	rememberScrolledToBottom();
	replaceLastMessage(newMessage);
	QWebElement replace = lastElement_.findFirst("span.swift_time");
	assert(!replace.isNull());
	replace.setInnerXml(ChatSnippet::escape(note));
}

void QtChatView::copySelectionToClipboard() {
	if (!webPage_->selectedText().isEmpty()) {
		webPage_->triggerAction(QWebPage::Copy);
	}
}

void QtChatView::setAckXML(const QString& id, const QString& xml) {
	QWebElement message = document_.findFirst("#" + id);
	/* Deliberately not asserting here, so that when we start expiring old messages it won't hit us */
	if (message.isNull()) return;
	QWebElement ackElement = message.findFirst("span.swift_ack");
	assert(!ackElement.isNull());
	ackElement.setInnerXml(xml);
}

void QtChatView::rememberScrolledToBottom() {
	isAtBottom_ = webPage_->mainFrame()->scrollBarValue(Qt::Vertical) == webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void QtChatView::scrollToBottom() {
	isAtBottom_ = true;
	webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical));
	webView_->update(); /* Work around redraw bug in some versions of Qt. */
}

void QtChatView::handleFrameSizeChanged() {
	if (isAtBottom_) {
		scrollToBottom();
	}
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
}

void QtChatView::resetView() {
	lastElement_ = QWebElement();
	QString pageHTML = theme_->getTemplate();
	pageHTML.replace("==bodyBackground==", "background-color:#e3e3e3");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, theme_->getBase());
	if (pageHTML.count("%@") > 3) {
		pageHTML.replace(pageHTML.indexOf("%@"), 2, theme_->getMainCSS());
	}
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "Variants/Blue on Green.css");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, ""/*headerSnippet.getContent()*/);
	pageHTML.replace(pageHTML.indexOf("%@"), 2, ""/*footerSnippet.getContent()*/);
	QEventLoop syncLoop;
	connect(webView_, SIGNAL(loadFinished(bool)), &syncLoop, SLOT(quit()));
	webPage_->mainFrame()->setHtml(pageHTML);
	while (!viewReady_) {
		QTimer t;
		t.setSingleShot(true);
		connect(&t, SIGNAL(timeout()), &syncLoop, SLOT(quit()));
		t.start(50);
		syncLoop.exec();
	}
	document_ = webPage_->mainFrame()->documentElement();
	QWebElement chatElement = document_.findFirst("#Chat");
	newInsertPoint_ = chatElement.clone();
	newInsertPoint_.setOuterXml("<div id='swift_insert'/>");
	chatElement.appendInside(newInsertPoint_);
	Q_ASSERT(!newInsertPoint_.isNull());

	connect(webPage_->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)), this, SLOT(handleFrameSizeChanged()), Qt::UniqueConnection);
}

}
