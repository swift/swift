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

#include <Swiften/Base/Log.h>

#include "QtWebView.h"
#include "QtChatTheme.h"
#include "QtChatWindow.h"
#include "QtSwiftUtil.h"


namespace Swift {

QtChatView::QtChatView(QtChatTheme* theme, QWidget* parent, bool disableAutoScroll) : QWidget(parent), fontSizeSteps_(0), disableAutoScroll_(disableAutoScroll) {
	theme_ = theme;

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);
	webView_ = new QtWebView(this);
	connect(webView_, SIGNAL(linkClicked(const QUrl&)), SLOT(handleLinkClicked(const QUrl&)));
	connect(webView_, SIGNAL(loadFinished(bool)), SLOT(handleViewLoadFinished(bool)));
	connect(webView_, SIGNAL(gotFocus()), SIGNAL(gotFocus()));
	connect(webView_, SIGNAL(clearRequested()), SLOT(handleClearRequested()));
	connect(webView_, SIGNAL(fontGrowRequested()), SLOT(increaseFontSize()));
	connect(webView_, SIGNAL(fontShrinkRequested()), SLOT(decreaseFontSize()));
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

#ifdef SWIFT_EXPERIMENTAL_FT
	setAcceptDrops(true);
#endif

	webPage_ = new QWebPage(this);
	webPage_->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	//webPage_->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	webView_->setPage(webPage_);
	connect(webPage_, SIGNAL(selectionChanged()), SLOT(copySelectionToClipboard()));
	connect(webPage_, SIGNAL(scrollRequested(int, int, const QRect&)), SLOT(handleScrollRequested(int, int, const QRect&)));

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
		logCleared();
		resetView();
	}
}

void QtChatView::handleKeyPressEvent(QKeyEvent* event) {
	webView_->keyPressEvent(event);
}

void QtChatView::addMessageBottom(boost::shared_ptr<ChatSnippet> snippet) {
	if (viewReady_) {
		addToDOM(snippet);
	} else {
		/* If this asserts, the previous queuing code was necessary and should be reinstated */
		assert(false);
	}
}

void QtChatView::addMessageTop(boost::shared_ptr<ChatSnippet> snippet) {
	// save scrollbar maximum value
	if (!topMessageAdded_) {
		scrollBarMaximum_ = webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical);
	}
	topMessageAdded_ = true;

	QWebElement continuationElement = firstElement_.findFirst("#insert");

	bool insert = snippet->getAppendToPrevious();
	bool fallback = continuationElement.isNull();

	boost::shared_ptr<ChatSnippet> newSnippet = (insert && fallback) ? snippet->getContinuationFallbackSnippet() : snippet;
	QWebElement newElement = snippetToDOM(newSnippet);

	if (insert && !fallback) {
		Q_ASSERT(!continuationElement.isNull());
		continuationElement.replace(newElement);
	} else {
		continuationElement.removeFromDocument();
		topInsertPoint_.prependOutside(newElement);
	}

	firstElement_ = newElement;

	if (lastElement_.isNull()) {
		lastElement_ = firstElement_;
	}

	if (fontSizeSteps_ != 0) {
		double size = 1.0 + 0.2 * fontSizeSteps_;
		QString sizeString(QString().setNum(size, 'g', 3) + "em");
		const QWebElementCollection spans = firstElement_.findAll("span.swift_resizable");
		foreach (QWebElement span, spans) {
			span.setStyleProperty("font-size", sizeString);
		}
	}
}

QWebElement QtChatView::snippetToDOM(boost::shared_ptr<ChatSnippet> snippet) {
	QWebElement newElement = newInsertPoint_.clone();
	newElement.setInnerXml(snippet->getContent());
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
	if (fontSizeSteps_ != 0) {
		double size = 1.0 + 0.2 * fontSizeSteps_;
		QString sizeString(QString().setNum(size, 'g', 3) + "em");
		const QWebElementCollection spans = lastElement_.findAll("span.swift_resizable");
		foreach (QWebElement span, spans) {
			span.setStyleProperty("font-size", sizeString);
		}
	}
}

void QtChatView::addLastSeenLine() {
	if (lineSeparator_.isNull()) {
		lineSeparator_ = newInsertPoint_.clone();
		lineSeparator_.setInnerXml(QString("<hr/>"));
		newInsertPoint_.prependOutside(lineSeparator_);
	}
	else {
		QWebElement lineSeparatorC = lineSeparator_.clone();
		lineSeparatorC.removeFromDocument();
	}
	newInsertPoint_.prependOutside(lineSeparator_);
}

void QtChatView::replaceLastMessage(const QString& newMessage) {
	assert(viewReady_);
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

QString QtChatView::getLastSentMessage() {
	return lastElement_.toPlainText();
}

void QtChatView::addToJSEnvironment(const QString& name, QObject* obj) {
	webView_->page()->currentFrame()->addToJavaScriptWindowObject(name, obj);
}

void QtChatView::replaceMessage(const QString& newMessage, const QString& id, const QDateTime& editTime) {
	rememberScrolledToBottom();
	QWebElement message = document_.findFirst("#" + id);
	if (!message.isNull()) {
		QWebElement replaceContent = message.findFirst("span.swift_inner_message");
		assert(!replaceContent.isNull());
		QString old = replaceContent.toOuterXml();
		replaceContent.setInnerXml(ChatSnippet::escape(newMessage));
		QWebElement replaceTime = message.findFirst("span.swift_time");
		assert(!replaceTime.isNull());
		old = replaceTime.toOuterXml();
		replaceTime.setInnerXml(ChatSnippet::escape(tr("%1 edited").arg(ChatSnippet::timeToEscapedString(editTime))));
	}
	else {
		qWarning() << "Trying to replace element with id " << id << " but it's not there.";
	}
}

void QtChatView::showEmoticons(bool show) {
	{
		const QWebElementCollection spans = document_.findAll("span.swift_emoticon_image");
		foreach (QWebElement span, spans) {
			span.setStyleProperty("display", show ? "inline" : "none");
		}
	}
	{
		const QWebElementCollection spans = document_.findAll("span.swift_emoticon_text");
		foreach (QWebElement span, spans) {
			span.setStyleProperty("display", show ? "none" : "inline");
		}
	}
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

void QtChatView::setReceiptXML(const QString& id, const QString& xml) {
	QWebElement message = document_.findFirst("#" + id);
	if (message.isNull()) return;
	QWebElement receiptElement = message.findFirst("span.swift_receipt");
	assert(!receiptElement.isNull());
	receiptElement.setInnerXml(xml);
}

void QtChatView::displayReceiptInfo(const QString& id, bool showIt) {
	QWebElement message = document_.findFirst("#" + id);
	if (message.isNull()) return;
	QWebElement receiptElement = message.findFirst("span.swift_receipt");
	assert(!receiptElement.isNull());
	receiptElement.setStyleProperty("display", showIt ? "inline" : "none");
}

void QtChatView::rememberScrolledToBottom() {
	isAtBottom_ = webPage_->mainFrame()->scrollBarValue(Qt::Vertical) >= (webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical) - 1);
}

void QtChatView::scrollToBottom() {
	isAtBottom_ = true;
	webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical));
	webView_->update(); /* Work around redraw bug in some versions of Qt. */
}

void QtChatView::handleFrameSizeChanged() {
	if (topMessageAdded_) {
		// adjust new scrollbar position
		int newMaximum = webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical);
		webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, newMaximum - scrollBarMaximum_);
		topMessageAdded_ = false;
	}

	if (isAtBottom_ && !disableAutoScroll_) {
		scrollToBottom();
	}
}

void QtChatView::handleLinkClicked(const QUrl& url) {
	QDesktopServices::openUrl(url);
}

void QtChatView::handleViewLoadFinished(bool ok) {
	Q_ASSERT(ok);
	viewReady_ = true;
}

void QtChatView::increaseFontSize(int numSteps) {
	//qDebug() << "Increasing";
	fontSizeSteps_ += numSteps;
	emit fontResized(fontSizeSteps_);
}

void QtChatView::decreaseFontSize() {
	fontSizeSteps_--;
	if (fontSizeSteps_ < 0) {
		fontSizeSteps_ = 0;
	}
	emit fontResized(fontSizeSteps_);
}

void QtChatView::resizeFont(int fontSizeSteps) {
	fontSizeSteps_ = fontSizeSteps;
	double size = 1.0 + 0.2 * fontSizeSteps_;
	QString sizeString(QString().setNum(size, 'g', 3) + "em");
	//qDebug() << "Setting to " << sizeString;
	const QWebElementCollection spans = document_.findAll("span.swift_resizable");
	foreach (QWebElement span, spans) {
		span.setStyleProperty("font-size", sizeString);
	}
	webView_->setFontSizeIsMinimal(size == 1.0);
}

void QtChatView::resetView() {
	lastElement_ = QWebElement();
	firstElement_ = lastElement_;
	topMessageAdded_ = false;
	scrollBarMaximum_ = 0;
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

	resetTopInsertPoint();
	QWebElement chatElement = document_.findFirst("#Chat");
	newInsertPoint_ = chatElement.clone();
	newInsertPoint_.setOuterXml("<div id='swift_insert'/>");
	chatElement.appendInside(newInsertPoint_);
	Q_ASSERT(!newInsertPoint_.isNull());

	scrollToBottom();

	connect(webPage_->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)), this, SLOT(handleFrameSizeChanged()), Qt::UniqueConnection);
}

QWebElement findDivElementWithID(QWebElement document, QString id) {
	QWebElementCollection divs = document.findAll("div");
	foreach(QWebElement div, divs) {
		if (div.attribute("id") == id) {
			return div;
		}
	}
	return QWebElement();
}

void QtChatView::setFileTransferProgress(QString id, const int percentageDone) {
	QWebElement ftElement = findDivElementWithID(document_, id);
	if (ftElement.isNull()) {
		SWIFT_LOG(debug) << "Tried to access FT UI via invalid id!" << std::endl;
		return;
	}
	QWebElement progressBar = ftElement.findFirst("div.progressbar");
	progressBar.setStyleProperty("width", QString::number(percentageDone) + "%");

	QWebElement progressBarValue = ftElement.findFirst("div.progressbar-value");
	progressBarValue.setInnerXml(QString::number(percentageDone) + " %");
}

void QtChatView::setFileTransferStatus(QString id, const ChatWindow::FileTransferState state, const QString& /* msg */) {
	QWebElement ftElement = findDivElementWithID(document_, id);
	if (ftElement.isNull()) {
		SWIFT_LOG(debug) << "Tried to access FT UI via invalid id! id = " << Q2PSTRING(id) << std::endl;
		return;
	}

	QString newInnerHTML = "";
	if (state == ChatWindow::WaitingForAccept) {
		newInnerHTML =	tr("Waiting for other side to accept the transfer.") + "<br/>" +
			QtChatWindow::buildChatWindowButton(tr("Cancel"), QtChatWindow::ButtonFileTransferCancel, id);
	}
	if (state == ChatWindow::Negotiating) {
		// replace with text "Negotiaging" + Cancel button
		newInnerHTML =	tr("Negotiating...") + "<br/>" +
			QtChatWindow::buildChatWindowButton(tr("Cancel"), QtChatWindow::ButtonFileTransferCancel, id);
	}
	else if (state == ChatWindow::Transferring) {
		// progress bar + Cancel Button
		newInnerHTML =	"<div style=\"position: relative; width: 90%; height: 20px; border: 2px solid grey; -webkit-border-radius: 10px;\">"
							"<div class=\"progressbar\" style=\"width: 0%; height: 100%; background: #AAA; -webkit-border-radius: 6px;\">"
								"<div class=\"progressbar-value\" style=\"position: absolute; top: 0px; left: 0px; width: 100%; text-align: center; padding-top: 2px;\">"
									"0%"
								"</div>"
							"</div>"
						"</div>" +
						QtChatWindow::buildChatWindowButton(tr("Cancel"), QtChatWindow::ButtonFileTransferCancel, id);
	}
	else if (state == ChatWindow::Canceled) {
		newInnerHTML = tr("Transfer has been canceled!");
	}
	else if (state == ChatWindow::Finished) {
		// text "Successful transfer"
		newInnerHTML = tr("Transfer completed successfully.");
	}
	else if (state == ChatWindow::FTFailed) {
		newInnerHTML = tr("Transfer failed.");
	}

	ftElement.setInnerXml(newInnerHTML);
}

void QtChatView::setMUCInvitationJoined(QString id) {
	QWebElement divElement = findDivElementWithID(document_, id);
	QWebElement buttonElement = divElement.findFirst("input#mucinvite");
	if (!buttonElement.isNull()) {
		buttonElement.setAttribute("value", tr("Return to room"));
	}
}

void QtChatView::handleScrollRequested(int, int dy, const QRect&) {
	rememberScrolledToBottom();

	int pos = webPage_->mainFrame()->scrollBarValue(Qt::Vertical) - dy;
	emit scrollRequested(pos);

	if (pos == 0) {
		emit scrollReachedTop();
	}
	else if (pos == webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical)) {
		emit scrollReachedBottom();
	}
}

int QtChatView::getSnippetPositionByDate(const QDate& date) {
	QWebElement message = webPage_->mainFrame()->documentElement().findFirst(".date" + date.toString(Qt::ISODate));

	return message.geometry().top();
}

void QtChatView::resetTopInsertPoint() {
	QWebElement continuationElement = firstElement_.findFirst("#insert");
	continuationElement.removeFromDocument();
	firstElement_ = QWebElement();

	topInsertPoint_.removeFromDocument();
	QWebElement chatElement = document_.findFirst("#Chat");
	topInsertPoint_ = chatElement.clone();
	topInsertPoint_.setOuterXml("<div id='swift_insert'/>");
	chatElement.prependInside(topInsertPoint_);
}

}
