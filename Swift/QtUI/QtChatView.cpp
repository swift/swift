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

namespace Swift {

QtChatView::QtChatView(QWidget* parent) : QWidget(parent) {
	setFocusPolicy(Qt::NoFocus);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);
	webView_ = new QtWebView(this);
	webView_->setFocusPolicy(Qt::NoFocus);
	connect(webView_, SIGNAL(linkClicked(const QUrl&)), SLOT(handleLinkClicked(const QUrl&)));
	connect(webView_, SIGNAL(loadFinished(bool)), SLOT(handleViewLoadFinished(bool)));
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

	QFile file(":/themes/Default/Template.html");
	bool result = file.open(QIODevice::ReadOnly);
	Q_ASSERT(result);
	Q_UNUSED(result);
	QString pageHTML = file.readAll();
	pageHTML.replace("==bodyBackground==", "background-color:#e3e3e3");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "qrc:/themes/Default/");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "Variants/Blue on Green.css");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "");
	pageHTML.replace(pageHTML.indexOf("%@"), 2, "");
	file.close();

	viewReady_ = false;
	webPage_->mainFrame()->setHtml(pageHTML);
}

void QtChatView::handleKeyPressEvent(QKeyEvent* event) {
	webView_->keyPressEvent(event);
}

void QtChatView::addMessage(const ChatSnippet& snippet) {
	//bool wasScrolledToBottom = isScrolledToBottom();
	
	QString content = snippet.getContent();
	content.replace("\\", "\\\\");
	content.replace("\"", "\\\"");
	content.replace("\n", "\\n");
	content.replace("\r", "");
	QString command;
	if (previousContinuationElementID_.isEmpty() || !snippet.getAppendToPrevious()) {
		command = "appendMessage(\"" + content + "\");";
	}
	else {
		command = "appendNextMessage(\"" + content + "\");";
	}
	if (viewReady_) {
		webPage_->mainFrame()->evaluateJavaScript(command);
	}
	else {
		queuedMessages_ += command;
	}

	//qDebug() << webPage_->mainFrame()->toHtml();
	previousContinuationElementID_ = snippet.getContinuationElementID();

	/*if (wasScrolledToBottom) {
		scrollToBottom();
	}*/
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

void QtChatView::handleViewLoadFinished(bool ok) {
	Q_ASSERT(ok);
	viewReady_ = true;
	webPage_->mainFrame()->evaluateJavaScript(queuedMessages_);
	queuedMessages_.clear();
}

}
