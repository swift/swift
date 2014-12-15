/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtWebKitChatView.h"

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
#include <QInputDialog>
#include <QFileDialog>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/FileSize.h>
#include <Swiften/StringCodecs/Base64.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>

#include <Swift/QtUI/QtWebView.h>
#include <Swift/QtUI/QtChatWindow.h>
#include <Swift/QtUI/QtChatWindowJSBridge.h>
#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/MessageSnippet.h>
#include <Swift/QtUI/SystemMessageSnippet.h>

namespace Swift {

const QString QtWebKitChatView::ButtonWhiteboardSessionCancel = QString("whiteboard-cancel");
const QString QtWebKitChatView::ButtonWhiteboardSessionAcceptRequest = QString("whiteboard-acceptrequest");
const QString QtWebKitChatView::ButtonWhiteboardShowWindow = QString("whiteboard-showwindow");
const QString QtWebKitChatView::ButtonFileTransferCancel = QString("filetransfer-cancel");
const QString QtWebKitChatView::ButtonFileTransferSetDescription = QString("filetransfer-setdescription");
const QString QtWebKitChatView::ButtonFileTransferSendRequest = QString("filetransfer-sendrequest");
const QString QtWebKitChatView::ButtonFileTransferAcceptRequest = QString("filetransfer-acceptrequest");
const QString QtWebKitChatView::ButtonMUCInvite = QString("mucinvite");

QtWebKitChatView::QtWebKitChatView(QtChatWindow* window, UIEventStream* eventStream, QtChatTheme* theme, QWidget* parent, bool disableAutoScroll) : QtChatView(parent), window_(window), eventStream_(eventStream), fontSizeSteps_(0), disableAutoScroll_(disableAutoScroll), previousMessageKind_(PreviosuMessageWasNone), previousMessageWasSelf_(false), showEmoticons_(false), insertingLastLine_(false), idCounter_(0) {
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
#if defined (Q_OS_UNIX) && !defined(Q_OS_MAC)
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
	if (Log::getLogLevel() == Log::debug) {
		webPage_->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	}
	webView_->setPage(webPage_);
	connect(webPage_, SIGNAL(selectionChanged()), SLOT(copySelectionToClipboard()));
	connect(webPage_, SIGNAL(scrollRequested(int, int, const QRect&)), SLOT(handleScrollRequested(int, int, const QRect&)));

	viewReady_ = false;
	isAtBottom_ = true;
	resetView();

	jsBridge = new QtChatWindowJSBridge();
	addToJSEnvironment("chatwindow", jsBridge);
	connect(jsBridge, SIGNAL(buttonClicked(QString,QString,QString,QString,QString,QString)), this, SLOT(handleHTMLButtonClicked(QString,QString,QString,QString,QString,QString)));

}

QtWebKitChatView::~QtWebKitChatView() {
	delete jsBridge;
}

void QtWebKitChatView::handleClearRequested() {
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

void QtWebKitChatView::handleKeyPressEvent(QKeyEvent* event) {
	webView_->keyPressEvent(event);
}

void QtWebKitChatView::addMessageBottom(boost::shared_ptr<ChatSnippet> snippet) {
	if (viewReady_) {
		addToDOM(snippet);
	} else {
		/* If this asserts, the previous queuing code was necessary and should be reinstated */
		assert(false);
	}
}

void QtWebKitChatView::addMessageTop(boost::shared_ptr<ChatSnippet> snippet) {
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
		Q_FOREACH (QWebElement span, spans) {
			span.setStyleProperty("font-size", sizeString);
		}
	}
}

QWebElement QtWebKitChatView::snippetToDOM(boost::shared_ptr<ChatSnippet> snippet) {
	QWebElement newElement = newInsertPoint_.clone();
	newElement.setInnerXml(snippet->getContent());
	Q_ASSERT(!newElement.isNull());
	return newElement;
}

void QtWebKitChatView::addToDOM(boost::shared_ptr<ChatSnippet> snippet) {
	//qDebug() << snippet->getContent();
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
		Q_FOREACH (QWebElement span, spans) {
			span.setStyleProperty("font-size", sizeString);
		}
	}
	//qDebug() << "-----------------";
	//qDebug() << webPage_->mainFrame()->toHtml();
}

void QtWebKitChatView::addLastSeenLine() {
	/* if the line is added we should break the snippet */
	insertingLastLine_ = true;
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

void QtWebKitChatView::replaceLastMessage(const QString& newMessage, const ChatWindow::TimestampBehaviour timestampBehaviour) {
	assert(viewReady_);
	rememberScrolledToBottom();
	assert(!lastElement_.isNull());
	QWebElement replace = lastElement_.findFirst("span.swift_message");
	assert(!replace.isNull());
	QString old = lastElement_.toOuterXml();
	replace.setInnerXml(ChatSnippet::escape(newMessage));
	if (timestampBehaviour == ChatWindow::UpdateTimestamp) {
		replace = lastElement_.findFirst("span.swift_time");
		assert(!replace.isNull());
		replace.setInnerXml(ChatSnippet::timeToEscapedString(QDateTime::currentDateTime()));
	}
}

void QtWebKitChatView::replaceLastMessage(const QString& newMessage, const QString& note) {
	rememberScrolledToBottom();
	replaceLastMessage(newMessage, ChatWindow::KeepTimestamp);
	QWebElement replace = lastElement_.findFirst("span.swift_time");
	assert(!replace.isNull());
	replace.setInnerXml(ChatSnippet::escape(note));
}

QString QtWebKitChatView::getLastSentMessage() {
	return lastElement_.toPlainText();
}

void QtWebKitChatView::addToJSEnvironment(const QString& name, QObject* obj) {
	webView_->page()->currentFrame()->addToJavaScriptWindowObject(name, obj);
}

void QtWebKitChatView::replaceMessage(const QString& newMessage, const QString& id, const QDateTime& editTime) {
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

void QtWebKitChatView::showEmoticons(bool show) {
	showEmoticons_ = show;
	{
		const QWebElementCollection spans = document_.findAll("span.swift_emoticon_image");
		Q_FOREACH (QWebElement span, spans) {
			span.setStyleProperty("display", show ? "inline" : "none");
		}
	}
	{
		const QWebElementCollection spans = document_.findAll("span.swift_emoticon_text");
		Q_FOREACH (QWebElement span, spans) {
			span.setStyleProperty("display", show ? "none" : "inline");
		}
	}
}

void QtWebKitChatView::copySelectionToClipboard() {
	if (!webPage_->selectedText().isEmpty()) {
		webPage_->triggerAction(QWebPage::Copy);
	}
}

void QtWebKitChatView::setAckXML(const QString& id, const QString& xml) {
	QWebElement message = document_.findFirst("#" + id);
	/* Deliberately not asserting here, so that when we start expiring old messages it won't hit us */
	if (message.isNull()) return;
	QWebElement ackElement = message.findFirst("span.swift_ack");
	assert(!ackElement.isNull());
	ackElement.setInnerXml(xml);
}

void QtWebKitChatView::setReceiptXML(const QString& id, const QString& xml) {
	QWebElement message = document_.findFirst("#" + id);
	if (message.isNull()) return;
	QWebElement receiptElement = message.findFirst("span.swift_receipt");
	assert(!receiptElement.isNull());
	receiptElement.setInnerXml(xml);
}

void QtWebKitChatView::displayReceiptInfo(const QString& id, bool showIt) {
	QWebElement message = document_.findFirst("#" + id);
	if (message.isNull()) return;
	QWebElement receiptElement = message.findFirst("span.swift_receipt");
	assert(!receiptElement.isNull());
	receiptElement.setStyleProperty("display", showIt ? "inline" : "none");
}

void QtWebKitChatView::rememberScrolledToBottom() {
	isAtBottom_ = webPage_->mainFrame()->scrollBarValue(Qt::Vertical) >= (webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical) - 1);
}

void QtWebKitChatView::scrollToBottom() {
	isAtBottom_ = true;
	webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical));
	webView_->update(); /* Work around redraw bug in some versions of Qt. */
}

void QtWebKitChatView::handleFrameSizeChanged() {
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

void QtWebKitChatView::handleLinkClicked(const QUrl& url) {
	QDesktopServices::openUrl(url);
}

void QtWebKitChatView::handleViewLoadFinished(bool ok) {
	Q_ASSERT(ok);
	viewReady_ = true;
}

void QtWebKitChatView::increaseFontSize(int numSteps) {
	//qDebug() << "Increasing";
	fontSizeSteps_ += numSteps;
	emit fontResized(fontSizeSteps_);
}

void QtWebKitChatView::decreaseFontSize() {
	fontSizeSteps_--;
	if (fontSizeSteps_ < 0) {
		fontSizeSteps_ = 0;
	}
	emit fontResized(fontSizeSteps_);
}

void QtWebKitChatView::resizeFont(int fontSizeSteps) {
	fontSizeSteps_ = fontSizeSteps;
	double size = 1.0 + 0.2 * fontSizeSteps_;
	QString sizeString(QString().setNum(size, 'g', 3) + "em");
	//qDebug() << "Setting to " << sizeString;
	const QWebElementCollection spans = document_.findAll("span.swift_resizable");
	Q_FOREACH (QWebElement span, spans) {
		span.setStyleProperty("font-size", sizeString);
	}
	webView_->setFontSizeIsMinimal(size == 1.0);
}

void QtWebKitChatView::resetView() {
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

static QWebElement findElementWithID(QWebElement document, QString elementName, QString id) {
	QWebElementCollection elements = document.findAll(elementName);
	Q_FOREACH(QWebElement element, elements) {
		if (element.attribute("id") == id) {
			return element;
		}
	}
	return QWebElement();
}

void QtWebKitChatView::setFileTransferProgress(QString id, const int percentageDone) {
	QWebElement ftElement = findElementWithID(document_, "div", id);
	if (ftElement.isNull()) {
		SWIFT_LOG(debug) << "Tried to access FT UI via invalid id!" << std::endl;
		return;
	}
	QWebElement progressBar = ftElement.findFirst("div.progressbar");
	progressBar.setStyleProperty("width", QString::number(percentageDone) + "%");

	QWebElement progressBarValue = ftElement.findFirst("div.progressbar-value");
	progressBarValue.setInnerXml(QString::number(percentageDone) + " %");
}

void QtWebKitChatView::setFileTransferStatus(QString id, const ChatWindow::FileTransferState state, const QString& /* msg */) {
	QWebElement ftElement = findElementWithID(document_, "div", id);
	if (ftElement.isNull()) {
		SWIFT_LOG(debug) << "Tried to access FT UI via invalid id! id = " << Q2PSTRING(id) << std::endl;
		return;
	}

	QString newInnerHTML = "";
	if (state == ChatWindow::WaitingForAccept) {
		newInnerHTML =	tr("Waiting for other side to accept the transfer.") + "<br/>" +
			buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
	}
	if (state == ChatWindow::Negotiating) {
		// replace with text "Negotiaging" + Cancel button
		newInnerHTML =	tr("Negotiating...") + "<br/>" +
			buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
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
						buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
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

void QtWebKitChatView::setWhiteboardSessionStatus(QString id, const ChatWindow::WhiteboardSessionState state) {
	QWebElement divElement = findElementWithID(document_, "div", id);
	QString newInnerHTML;
	if (state == ChatWindow::WhiteboardAccepted) {
		newInnerHTML =	tr("Started whiteboard chat") + "<br/>" + buildChatWindowButton(tr("Show whiteboard"), ButtonWhiteboardShowWindow, id);
	} else if (state == ChatWindow::WhiteboardTerminated) {
		newInnerHTML =	tr("Whiteboard chat has been canceled");
	} else if (state == ChatWindow::WhiteboardRejected) {
		newInnerHTML =	tr("Whiteboard chat request has been rejected");
	}
	divElement.setInnerXml(newInnerHTML);
}

void QtWebKitChatView::setMUCInvitationJoined(QString id) {
	QWebElement divElement = findElementWithID(document_, "div", id);
	QWebElement buttonElement = findElementWithID(divElement, "input", "mucinvite");
	if (!buttonElement.isNull()) {
		buttonElement.setAttribute("value", tr("Return to room"));
	}
}

void QtWebKitChatView::handleScrollRequested(int, int dy, const QRect&) {
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

int QtWebKitChatView::getSnippetPositionByDate(const QDate& date) {
	QWebElement message = webPage_->mainFrame()->documentElement().findFirst(".date" + date.toString(Qt::ISODate));

	return message.geometry().top();
}

void QtWebKitChatView::resetTopInsertPoint() {
	QWebElement continuationElement = firstElement_.findFirst("#insert");
	continuationElement.removeFromDocument();
	firstElement_ = QWebElement();

	topInsertPoint_.removeFromDocument();
	QWebElement chatElement = document_.findFirst("#Chat");
	topInsertPoint_ = chatElement.clone();
	topInsertPoint_.setOuterXml("<div id='swift_insert'/>");
	chatElement.prependInside(topInsertPoint_);
}


std::string QtWebKitChatView::addMessage(
		const ChatWindow::ChatMessage& message, 
		const std::string& senderName, 
		bool senderIsSelf, 
		boost::shared_ptr<SecurityLabel> label, 
		const std::string& avatarPath, 
		const boost::posix_time::ptime& time, 
		const HighlightAction& highlight) {
	return addMessage(chatMessageToHTML(message), senderName, senderIsSelf, label, avatarPath, "", time, highlight, ChatSnippet::getDirection(message));
}

QString QtWebKitChatView::getHighlightSpanStart(const std::string& text, const std::string& background) {
	QString ecsapeColor = QtUtilities::htmlEscape(P2QSTRING(text));
	QString escapeBackground = QtUtilities::htmlEscape(P2QSTRING(background));
	if (ecsapeColor.isEmpty()) {
		ecsapeColor = "black";
	}
	if (escapeBackground.isEmpty()) {
		escapeBackground = "yellow";
	}
	return QString("<span style=\"color: %1; background: %2\">").arg(ecsapeColor).arg(escapeBackground);
}

QString QtWebKitChatView::getHighlightSpanStart(const HighlightAction& highlight) {
	return getHighlightSpanStart(highlight.getTextColor(), highlight.getTextBackground());
}

QString QtWebKitChatView::chatMessageToHTML(const ChatWindow::ChatMessage& message) {
	QString result;
	foreach (boost::shared_ptr<ChatWindow::ChatMessagePart> part, message.getParts()) {
		boost::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
		boost::shared_ptr<ChatWindow::ChatURIMessagePart> uriPart;
		boost::shared_ptr<ChatWindow::ChatEmoticonMessagePart> emoticonPart;
		boost::shared_ptr<ChatWindow::ChatHighlightingMessagePart> highlightPart;

		if ((textPart = boost::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
			QString text = QtUtilities::htmlEscape(P2QSTRING(textPart->text));
			text.replace("\n","<br/>");
			result += text;
			continue;
		}
		if ((uriPart = boost::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(part))) {
			QString uri = QtUtilities::htmlEscape(P2QSTRING(uriPart->target));
			result += "<a href='" + uri + "' >" + uri + "</a>";
			continue;
		}
		if ((emoticonPart = boost::dynamic_pointer_cast<ChatWindow::ChatEmoticonMessagePart>(part))) {
			QString textStyle = showEmoticons_ ? "style='display:none'" : "";
			QString imageStyle = showEmoticons_ ? "" : "style='display:none'";
			result += "<span class='swift_emoticon_image' " + imageStyle + "><img src='" + P2QSTRING(emoticonPart->imagePath) + "'/></span><span class='swift_emoticon_text' " + textStyle + ">" + QtUtilities::htmlEscape(P2QSTRING(emoticonPart->alternativeText)) + "</span>";
			continue;
		}
		if ((highlightPart = boost::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part))) {
			QString spanStart = getHighlightSpanStart(highlightPart->foregroundColor, highlightPart->backgroundColor);
			result += spanStart + QtUtilities::htmlEscape(P2QSTRING(highlightPart->text)) + "</span>";
			continue;
		}

	}
	return result;
}

std::string QtWebKitChatView::addMessage(
		const QString& message, 
		const std::string& senderName, 
		bool senderIsSelf, 
		boost::shared_ptr<SecurityLabel> label, 
		const std::string& avatarPath, 
		const QString& style, 
		const boost::posix_time::ptime& time, 
		const HighlightAction& highlight,
		ChatSnippet::Direction direction) {

	QString scaledAvatarPath = QtScaledAvatarCache(32).getScaledAvatarPath(avatarPath.c_str());

	QString htmlString;
	if (label) {
		htmlString = QString("<span style=\"border: thin dashed grey; padding-left: .5em; padding-right: .5em; color: %1; background-color: %2; font-size: 90%; margin-right: .5em; \" class='swift_label'>").arg(QtUtilities::htmlEscape(P2QSTRING(label->getForegroundColor()))).arg(QtUtilities::htmlEscape(P2QSTRING(label->getBackgroundColor())));
		htmlString += QString("%1</span> ").arg(QtUtilities::htmlEscape(P2QSTRING(label->getDisplayMarking())));
	}

	QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
	QString styleSpanEnd = style == "" ? "" : "</span>";
	QString highlightSpanStart = highlight.highlightAllText() ? getHighlightSpanStart(highlight) : "";
	QString highlightSpanEnd = highlight.highlightAllText() ? "</span>" : "";
	htmlString += "<span class='swift_inner_message'>" + styleSpanStart + highlightSpanStart + message + highlightSpanEnd + styleSpanEnd + "</span>" ;

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasMessage, senderName, senderIsSelf);

	QString qAvatarPath =  scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.png" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
	std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
	addMessageBottom(boost::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(time), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id), direction));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasMessage;
	return id;
}

std::string QtWebKitChatView::addAction(const ChatWindow::ChatMessage& message, const std::string &senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	return addMessage(" *" + chatMessageToHTML(message) + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ", time, highlight, ChatSnippet::getDirection(message));
}

static QString encodeButtonArgument(const QString& str) {
	return QtUtilities::htmlEscape(P2QSTRING(Base64::encode(createByteArray(Q2PSTRING(str)))));
}

static QString decodeButtonArgument(const QString& str) {
	return P2QSTRING(byteArrayToString(Base64::decode(Q2PSTRING(str))));
}

QString QtWebKitChatView::buildChatWindowButton(const QString& name, const QString& id, const QString& arg1, const QString& arg2, const QString& arg3, const QString& arg4, const QString& arg5) {
	QRegExp regex("[A-Za-z][A-Za-z0-9\\-\\_]+");
	Q_ASSERT(regex.exactMatch(id));
	QString html = QString("<input id='%2' type='submit' value='%1' onclick='chatwindow.buttonClicked(\"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\");' />").arg(name).arg(id).arg(encodeButtonArgument(arg1)).arg(encodeButtonArgument(arg2)).arg(encodeButtonArgument(arg3)).arg(encodeButtonArgument(arg4)).arg(encodeButtonArgument(arg5));
	return html;
}

std::string QtWebKitChatView::addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes) {
	SWIFT_LOG(debug) << "addFileTransfer" << std::endl;
	QString ft_id = QString("ft%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));
	
	QString actionText;
	QString htmlString;
	QString formattedFileSize = P2QSTRING(formatSize(sizeInBytes));
	if (senderIsSelf) {
		// outgoing
		actionText = tr("Send file");
		htmlString = actionText + ": " + P2QSTRING(filename) + " ( " + formattedFileSize + ") <br/>" +
			"<div id='" + ft_id + "'>" +
				buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
				buildChatWindowButton(tr("Set Description"), ButtonFileTransferSetDescription, ft_id) +
				buildChatWindowButton(tr("Send"), ButtonFileTransferSendRequest, ft_id) +
			"</div>";
	} else {
		// incoming
		actionText = tr("Receiving file");
		htmlString = actionText + ": " + P2QSTRING(filename) + " ( " + formattedFileSize  + ") <br/>" +
			"<div id='" + ft_id + "'>" +
				buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
				buildChatWindowButton(tr("Accept"), ButtonFileTransferAcceptRequest, ft_id, P2QSTRING(filename)) +
			"</div>";
	}

	//addMessage(message, senderName, senderIsSelf, boost::shared_ptr<SecurityLabel>(), "", boost::posix_time::second_clock::local_time());

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasFileTransfer, senderName, senderIsSelf);

	QString qAvatarPath = "qrc:/icons/avatar.png";
	std::string id = "ftmessage" + boost::lexical_cast<std::string>(idCounter_++);
	addMessageBottom(boost::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id), ChatSnippet::getDirection(actionText)));

	previousMessageWasSelf_ = senderIsSelf;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasFileTransfer;
	return Q2PSTRING(ft_id);
}

void QtWebKitChatView::setFileTransferProgress(std::string id, const int percentageDone) {
	setFileTransferProgress(P2QSTRING(id), percentageDone);
}

void QtWebKitChatView::setFileTransferStatus(std::string id, const ChatWindow::FileTransferState state, const std::string& msg) {
	setFileTransferStatus(P2QSTRING(id), state, P2QSTRING(msg));
}

std::string QtWebKitChatView::addWhiteboardRequest(const QString& contact, bool senderIsSelf) {
	QString wb_id = QString("wb%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));
	QString htmlString;
	QString actionText;
	if (senderIsSelf) {
		actionText = tr("Starting whiteboard chat");
		htmlString = "<div id='" + wb_id + "'>" + actionText + "<br />"+
				buildChatWindowButton(tr("Cancel"), ButtonWhiteboardSessionCancel, wb_id) +
			"</div>";
	} else {
		actionText = tr("%1 would like to start a whiteboard chat");
		htmlString = "<div id='" + wb_id + "'>" + actionText.arg(QtUtilities::htmlEscape(contact)) + ": <br/>" +
				buildChatWindowButton(tr("Cancel"), ButtonWhiteboardSessionCancel, wb_id) +
				buildChatWindowButton(tr("Accept"), ButtonWhiteboardSessionAcceptRequest, wb_id) +
			"</div>";
	}
	QString qAvatarPath = "qrc:/icons/avatar.png";
	std::string id = "wbmessage" + boost::lexical_cast<std::string>(idCounter_++);
	addMessageBottom(boost::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(contact), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, false, false, theme_, P2QSTRING(id), ChatSnippet::getDirection(actionText)));
	previousMessageWasSelf_ = false;
	previousSenderName_ = contact;
	return Q2PSTRING(wb_id);
}

void QtWebKitChatView::setWhiteboardSessionStatus(const std::string& id, const ChatWindow::WhiteboardSessionState state) {
	setWhiteboardSessionStatus(P2QSTRING(id), state);
}




void QtWebKitChatView::handleHTMLButtonClicked(QString id, QString encodedArgument1, QString encodedArgument2, QString encodedArgument3, QString encodedArgument4, QString encodedArgument5) {
	QString arg1 = decodeButtonArgument(encodedArgument1);
	QString arg2 = decodeButtonArgument(encodedArgument2);
	QString arg3 = decodeButtonArgument(encodedArgument3);
	QString arg4 = decodeButtonArgument(encodedArgument4);
	QString arg5 = decodeButtonArgument(encodedArgument5);

	if (id.startsWith(ButtonFileTransferCancel)) {
		QString ft_id = arg1;
		window_->onFileTransferCancel(Q2PSTRING(ft_id));
	}
	else if (id.startsWith(ButtonFileTransferSetDescription)) {
		QString ft_id = arg1;
		bool ok = false;
		QString text = QInputDialog::getText(this, tr("File transfer description"),
			tr("Description:"), QLineEdit::Normal, "", &ok);
		if (ok) {
			descriptions_[ft_id] = text;
		}
	}
	else if (id.startsWith(ButtonFileTransferSendRequest)) {
		QString ft_id = arg1;
		QString text = descriptions_.find(ft_id) == descriptions_.end() ? QString() : descriptions_[ft_id];
		window_->onFileTransferStart(Q2PSTRING(ft_id), Q2PSTRING(text));
	}
	else if (id.startsWith(ButtonFileTransferAcceptRequest)) {
		QString ft_id = arg1;
		QString filename = arg2;

		QString path = QFileDialog::getSaveFileName(this, tr("Save File"), filename);
		if (!path.isEmpty()) {
			window_->onFileTransferAccept(Q2PSTRING(ft_id), Q2PSTRING(path));
		}
	}
	else if (id.startsWith(ButtonWhiteboardSessionAcceptRequest)) {
		QString id = arg1;
		setWhiteboardSessionStatus(id, ChatWindow::WhiteboardAccepted);
		window_->onWhiteboardSessionAccept();
	}
	else if (id.startsWith(ButtonWhiteboardSessionCancel)) {
		QString id = arg1;
		setWhiteboardSessionStatus(id, ChatWindow::WhiteboardTerminated);
		window_->onWhiteboardSessionCancel();
	}
	else if (id.startsWith(ButtonWhiteboardShowWindow)) {
		QString id = arg1;
		window_->onWhiteboardWindowShow();
	}
	else if (id.startsWith(ButtonMUCInvite)) {
		QString roomJID = arg1;
		QString password = arg2;
		QString elementID = arg3;
		QString isImpromptu = arg4;
		QString isContinuation = arg5;
		eventStream_->send(boost::make_shared<JoinMUCUIEvent>(Q2PSTRING(roomJID), Q2PSTRING(password), boost::optional<std::string>(), false, false, isImpromptu.contains("true"), isContinuation.contains("true")));
		setMUCInvitationJoined(elementID);
	}
	else {
		SWIFT_LOG(debug) << "Unknown HTML button! ( " << Q2PSTRING(id) << " )" << std::endl;
	}
}

void QtWebKitChatView::addErrorMessage(const ChatWindow::ChatMessage& errorMessage) {
	if (window_->isWidgetSelected()) {
		window_->onAllMessagesRead();
	}

	QString errorMessageHTML(chatMessageToHTML(errorMessage));
	
	addMessageBottom(boost::make_shared<SystemMessageSnippet>("<span class=\"error\">" + errorMessageHTML + "</span>", QDateTime::currentDateTime(), false, theme_, ChatSnippet::getDirection(errorMessage)));

	previousMessageWasSelf_ = false;
	previousMessageKind_ = PreviousMessageWasSystem;
}

void QtWebKitChatView::addSystemMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) {
	if (window_->isWidgetSelected()) {
		window_->onAllMessagesRead();
	}

	QString messageHTML = chatMessageToHTML(message);
	addMessageBottom(boost::make_shared<SystemMessageSnippet>(messageHTML, QDateTime::currentDateTime(), false, theme_, getActualDirection(message, direction)));

	previousMessageKind_ = PreviousMessageWasSystem;
}

void QtWebKitChatView::replaceWithAction(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	replaceMessage(" *" + chatMessageToHTML(message) + "*", id, time, "font-style:italic ", highlight);
}

void QtWebKitChatView::replaceMessage(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	replaceMessage(chatMessageToHTML(message), id, time, "", highlight);
}

void QtWebKitChatView::replaceMessage(const QString& message, const std::string& id, const boost::posix_time::ptime& time, const QString& style, const HighlightAction& highlight) {
	if (!id.empty()) {
		if (window_->isWidgetSelected()) {
			window_->onAllMessagesRead();
		}

		QString messageHTML(message);

		QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
		QString styleSpanEnd = style == "" ? "" : "</span>";
		QString highlightSpanStart = highlight.highlightAllText() ? getHighlightSpanStart(highlight) : "";
		QString highlightSpanEnd = highlight.highlightAllText() ? "</span>" : "";
		messageHTML = styleSpanStart + highlightSpanStart + messageHTML + highlightSpanEnd + styleSpanEnd;

		replaceMessage(messageHTML, P2QSTRING(id), B2QDATE(time));
	}
	else {
		std::cerr << "Trying to replace a message with no id";
	}
}

void QtWebKitChatView::addPresenceMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) {
	if (window_->isWidgetSelected()) {
		window_->onAllMessagesRead();
	}

	QString messageHTML = chatMessageToHTML(message);
	addMessageBottom(boost::make_shared<SystemMessageSnippet>(messageHTML, QDateTime::currentDateTime(), false, theme_, getActualDirection(message, direction)));

	previousMessageKind_ = PreviousMessageWasPresence;
}

void QtWebKitChatView::replaceLastMessage(const ChatWindow::ChatMessage& message, const ChatWindow::TimestampBehaviour timestampBehaviour) {
	replaceLastMessage(chatMessageToHTML(message), timestampBehaviour);
}

void QtWebKitChatView::addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct, bool isImpromptu, bool isContinuation) {
	if (window_->isWidgetSelected()) {
		window_->onAllMessagesRead();
	}

	QString message;
	if (isImpromptu) {
		message = QObject::tr("You've been invited to join a chat.") + "\n";
	} else {
		message = QObject::tr("You've been invited to enter the %1 room.").arg(P2QSTRING(jid.toString())) + "\n";
	}
	QString htmlString = message;
	if (!reason.empty()) {
		htmlString += QObject::tr("Reason: %1").arg(P2QSTRING(reason)) + "\n";
	}
	if (!direct) {
		htmlString += QObject::tr("This person may not have really sent this invitation!") + "\n";
	}
	htmlString = chatMessageToHTML(ChatWindow::ChatMessage(Q2PSTRING(htmlString)));


	QString id = QString(ButtonMUCInvite + "%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));
	htmlString += "<div id='" + id + "'>" +
			buildChatWindowButton(chatMessageToHTML(ChatWindow::ChatMessage(Q2PSTRING((tr("Accept Invite"))))), ButtonMUCInvite, QtUtilities::htmlEscape(P2QSTRING(jid.toString())), QtUtilities::htmlEscape(P2QSTRING(password)), id, QtUtilities::htmlEscape(isImpromptu ? "true" : "false"), QtUtilities::htmlEscape(isContinuation ? "true" : "false")) +
		"</div>";

	bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasMUCInvite, senderName, false);

	QString qAvatarPath = "qrc:/icons/avatar.png";

	addMessageBottom(boost::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::local_time()), qAvatarPath, false, appendToPrevious, theme_, id, ChatSnippet::getDirection(message)));
	previousMessageWasSelf_ = false;
	previousSenderName_ = P2QSTRING(senderName);
	previousMessageKind_ = PreviousMessageWasMUCInvite;
}

void QtWebKitChatView::setAckState(std::string const& id, ChatWindow::AckState state) {
	QString xml;
	switch (state) {
		case ChatWindow::Pending:
			xml = "<img src='qrc:/icons/throbber.gif' title='" + tr("This message has not been received by your server yet.") + "'/>";
			displayReceiptInfo(P2QSTRING(id), false);
			break;
		case ChatWindow::Received:
			xml = "";
			displayReceiptInfo(P2QSTRING(id), true);
			break;
		case ChatWindow::Failed: xml = "<img src='qrc:/icons/error.png' title='" + tr("This message may not have been transmitted.") + "'/>"; break;
	}
	setAckXML(P2QSTRING(id), xml);
}

void QtWebKitChatView::setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) {
	QString xml;
	switch (state) {
		case ChatWindow::ReceiptReceived:
			xml = "<img src='qrc:/icons/check.png' title='" + tr("The receipt for this message has been received.") + "'/>";
			break;
		case ChatWindow::ReceiptRequested:
			xml = "<img src='qrc:/icons/warn.png' title='" + tr("The receipt for this message has not yet been received. The recipient(s) might not have received this message.") + "'/>";
			break;
		case ChatWindow::ReceiptFailed:
			xml = "<img src='qrc:/icons/error.png' title='" + tr("Failed to transmit message to the receipient(s).") + "'/>";
	}
	setReceiptXML(P2QSTRING(id), xml);
}

bool QtWebKitChatView::appendToPreviousCheck(PreviousMessageKind messageKind, const std::string& senderName, bool senderIsSelf) {
	bool result = previousMessageKind_ == messageKind && ((senderIsSelf && previousMessageWasSelf_) || (!senderIsSelf && !previousMessageWasSelf_&& previousSenderName_ == P2QSTRING(senderName)));
	if (insertingLastLine_) {
		insertingLastLine_ = false;
		return false;
	}
	return result;
}

ChatSnippet::Direction QtWebKitChatView::getActualDirection(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) {
	if (direction == ChatWindow::DefaultDirection) {
		return QCoreApplication::translate("QApplication", "QT_LAYOUT_DIRECTION") == "RTL" ? ChatSnippet::RTL : ChatSnippet::LTR;
	}
	else {
		return ChatSnippet::getDirection(message);
	}
}

// void QtWebKitChatView::setShowEmoticons(bool value) {
// 	showEmoticons_ = value;
// }


}
