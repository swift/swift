/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtWebKitChatView.h>

#include <QApplication>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileDevice>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QWebSettings>
#include <QtDebug>

#include <Swiften/Base/FileSize.h>
#include <Swiften/Base/Log.h>
#include <Swiften/StringCodecs/Base64.h>

#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/MessageSnippet.h>
#include <Swift/QtUI/QtChatWindow.h>
#include <Swift/QtUI/QtChatWindowJSBridge.h>
#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/QtWebView.h>
#include <Swift/QtUI/SystemMessageSnippet.h>

namespace Swift {

const QString QtWebKitChatView::ButtonWhiteboardSessionCancel = QString("whiteboard-cancel");
const QString QtWebKitChatView::ButtonWhiteboardSessionAcceptRequest = QString("whiteboard-acceptrequest");
const QString QtWebKitChatView::ButtonWhiteboardShowWindow = QString("whiteboard-showwindow");
const QString QtWebKitChatView::ButtonFileTransferCancel = QString("filetransfer-cancel");
const QString QtWebKitChatView::ButtonFileTransferSetDescription = QString("filetransfer-setdescription");
const QString QtWebKitChatView::ButtonFileTransferSendRequest = QString("filetransfer-sendrequest");
const QString QtWebKitChatView::ButtonFileTransferAcceptRequest = QString("filetransfer-acceptrequest");
const QString QtWebKitChatView::ButtonFileTransferOpenFile = QString("filetransfer-openfile");
const QString QtWebKitChatView::ButtonMUCInvite = QString("mucinvite");

namespace {
    const double minimalFontScaling = 0.7;
}

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

    viewReady_ = false;
    isAtBottom_ = true;
    resetView();

    jsBridge = new QtChatWindowJSBridge();
    addToJSEnvironment("chatwindow", jsBridge);
    connect(jsBridge, SIGNAL(buttonClicked(QString,QString,QString,QString,QString,QString)), this, SLOT(handleHTMLButtonClicked(QString,QString,QString,QString,QString,QString)));
    connect(jsBridge, SIGNAL(verticalScrollBarPositionChanged(double)), this, SLOT(handleVerticalScrollBarPositionChanged(double)));
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
        resetView();
        logCleared();
        resizeFont(fontSizeSteps_);
    }
}

void QtWebKitChatView::handleKeyPressEvent(QKeyEvent* event) {
    webView_->keyPressEvent(event);
}

void QtWebKitChatView::addMessageBottom(std::shared_ptr<ChatSnippet> snippet) {
    if (viewReady_) {
        addToDOM(snippet);
    } else {
        /* If this asserts, the previous queuing code was necessary and should be reinstated */
        assert(false);
    }
}

void QtWebKitChatView::addMessageTop(std::shared_ptr<ChatSnippet> /* snippet */) {
    // TODO: Implement this in a sensible manner later.
    SWIFT_LOG(error) << "Not yet implemented!" << std::endl;
}

void QtWebKitChatView::addToDOM(std::shared_ptr<ChatSnippet> snippet) {
    //qDebug() << snippet->getContent();
    rememberScrolledToBottom();

    QWebElement insertElement = webPage_->mainFrame()->findFirstElement("#insert");
    assert(!insertElement.isNull());
    insertElement.prependOutside(snippet->getContent());

    //qDebug() << "-----------------";
    //qDebug() << webPage_->mainFrame()->toHtml();
}

void QtWebKitChatView::addLastSeenLine() {
    // Remove a potentially existing unread bar.
    QWebElement existingUnreadBar = webPage_->mainFrame()->findFirstElement("div.unread");
    if (!existingUnreadBar.isNull()) {
        existingUnreadBar.removeFromDocument();
    }

    QWebElement insertElement = webPage_->mainFrame()->findFirstElement("#insert");
    insertElement.prependOutside(theme_->getUnread());
}

void QtWebKitChatView::replaceLastMessage(const QString& newMessage, const ChatWindow::TimestampBehaviour timestampBehaviour) {
    rememberScrolledToBottom();
    QWebElement insertElement = webPage_->mainFrame()->findFirstElement("#insert");
    assert(!insertElement.isNull());

    QWebElement lastMessageElement = insertElement.previousSibling();
    QWebElement messageChild = lastMessageElement.findFirst("span.swift_message");
    assert(!messageChild.isNull());
    messageChild.setInnerXml(ChatSnippet::escape(newMessage));
    if (timestampBehaviour == ChatWindow::UpdateTimestamp) {
        QWebElement timeChild = lastMessageElement.findFirst("span.swift_time");
        assert(!timeChild.isNull());
        timeChild.setInnerXml(ChatSnippet::timeToEscapedString(QDateTime::currentDateTime()));
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
    if (webPage_) {
        isAtBottom_ = webPage_->mainFrame()->scrollBarValue(Qt::Vertical) >= (webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical) - 1);
    }
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
    double size = minimalFontScaling + 0.1 * fontSizeSteps_;
    QString sizeString(QString().setNum(size, 'g', 3) + "em");

    // Set the font size in the <style id="text-resize-style"> element in the theme <head> element.
    QWebElement resizableTextStyle = document_.findFirst("style#text-resize-style");
    assert(!resizableTextStyle.isNull());
    resizableTextStyle.setInnerXml(QString("span.swift_resizable { font-size: %1;}").arg(sizeString));
    webView_->setFontSizeIsMinimal(size == minimalFontScaling);
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
        syncLoop.processEvents(QEventLoop::AllEvents, 50);
    }
    document_ = webPage_->mainFrame()->documentElement();

    scrollToBottom();

    connect(webPage_->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)), this, SLOT(handleFrameSizeChanged()), Qt::UniqueConnection);

    // Hooking up to scroll bar update, because Qt does not provide a way to retrieve accurate scroll bar updates from C++ directly.
    QWebElement body = document_.findFirst("body");
    assert(!body.isNull());
    body.setAttribute("onscroll", "chatwindow.verticalScrollBarPositionChanged(document.body.scrollTop / (document.body.scrollHeight - window.innerHeight))");

    // Adjust web view default 96 DPI setting to screen DPI.
    // For more information see https://webkit.org/blog/57/css-units/
    webView_->setZoomFactor(QApplication::desktop()->screen()->logicalDpiX() / 96.0);

    body.setStyleProperty("font-size", QString("%1pt").arg(QApplication::font().pointSize()));

#ifdef Q_OS_WIN
    // Fix for Swift-162
    // Changing the font weight for windows in order for the fonts to display better.
    // In the future, when we enable dpiawareness on windows, this can be reverted.
    body.setStyleProperty("font-weight", QString("500"));
#endif // Q_OS_WIN

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
    rememberScrolledToBottom();
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
    rememberScrolledToBottom();
    QWebElement ftElement = findElementWithID(document_, "div", id);
    if (ftElement.isNull()) {
        SWIFT_LOG(debug) << "Tried to access FT UI via invalid id! id = " << Q2PSTRING(id) << std::endl;
        return;
    }

    QString newInnerHTML = "";
    if (state == ChatWindow::Initialisation) {
        QWebElement filenameSizeDescriptionElement = ftElement.parent().firstChild();
        QString description = QtUtilities::htmlEscape(descriptions_[id]);
        if (!description.isEmpty()) {
            filenameSizeDescriptionElement.prependOutside(QString(" \"%1\"").arg(description));
        }
        newInnerHTML = tr("Preparing to transfer.") + "<br/>" +
            buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
    }
    else if (state == ChatWindow::WaitingForAccept) {
        newInnerHTML = tr("Waiting for other side to accept the transfer.") + "<br/>" +
            buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
    }
    else if (state == ChatWindow::Negotiating) {
        // replace with text "Negotiaging" + Cancel button
        newInnerHTML = tr("Negotiating...") + "<br/>" +
            buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, id);
    }
    else if (state == ChatWindow::Transferring) {
        // progress bar + Cancel Button
        newInnerHTML = "<div style=\"position: relative; width: 90%; height: 20px; border: 2px solid grey; -webkit-border-radius: 10px;\">"
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
        newInnerHTML = tr("Transfer completed successfully." ) + " " + buildChatWindowButton(tr("Open file"), ButtonFileTransferOpenFile, id, filePaths_[id]);
        filePaths_.erase(id);
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
        newInnerHTML =    tr("Started whiteboard chat") + "<br/>" + buildChatWindowButton(tr("Show whiteboard"), ButtonWhiteboardShowWindow, id);
    } else if (state == ChatWindow::WhiteboardTerminated) {
        newInnerHTML =    tr("Whiteboard chat has been canceled");
    } else if (state == ChatWindow::WhiteboardRejected) {
        newInnerHTML =    tr("Whiteboard chat request has been rejected");
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

void QtWebKitChatView::askDesktopToOpenFile(const QString& filename) {
    QFileInfo fileInfo(filename);
    if (fileInfo.exists() && fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    }
}

int QtWebKitChatView::getSnippetPositionByDate(const QDate& date) {
    QWebElement message = webPage_->mainFrame()->documentElement().findFirst(".date" + date.toString(Qt::ISODate));

    return message.geometry().top();
}

void QtWebKitChatView::resetTopInsertPoint() {
    // TODO: Implement or refactor later.
    SWIFT_LOG(error) << "Not yet implemented!" << std::endl;
}

std::string QtWebKitChatView::addMessage(
        const ChatWindow::ChatMessage& message,
        const std::string& senderName,
        bool senderIsSelf,
        std::shared_ptr<SecurityLabel> label,
        const std::string& avatarPath,
        const boost::posix_time::ptime& time) {
    return addMessage(chatMessageToHTML(message), senderName, senderIsSelf, label, avatarPath, "", time, message.getHighlightActionSender(), ChatSnippet::getDirection(message));
}

QString QtWebKitChatView::getHighlightSpanStart(const std::string& text, const std::string& background) {
    QString ecsapeColor;
    QString escapeBackground;
    if (!text.empty()) {
        ecsapeColor = QString("color: %1").arg(QtUtilities::htmlEscape(P2QSTRING(text)));
    }
    if (!background.empty()) {
        escapeBackground = QString("background: %1").arg(QtUtilities::htmlEscape(P2QSTRING(background)));
    }
    return QString("<span style=\"%1; %2;\">").arg(ecsapeColor).arg(escapeBackground);
}

QString QtWebKitChatView::getHighlightSpanStart(const HighlightAction& highlight) {
    return getHighlightSpanStart(highlight.getFrontColor().get_value_or(""), highlight.getBackColor().get_value_or(""));
}

QString QtWebKitChatView::chatMessageToHTML(const ChatWindow::ChatMessage& message) {
    QString result;
    for (const auto& part : message.getParts()) {
        std::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
        std::shared_ptr<ChatWindow::ChatURIMessagePart> uriPart;
        std::shared_ptr<ChatWindow::ChatEmoticonMessagePart> emoticonPart;
        std::shared_ptr<ChatWindow::ChatHighlightingMessagePart> highlightPart;

        if ((textPart = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
            QString text = QtUtilities::htmlEscape(P2QSTRING(textPart->text));
            text.replace("\n","<br/>");
            result += text;
            continue;
        }
        if ((uriPart = std::dynamic_pointer_cast<ChatWindow::ChatURIMessagePart>(part))) {
            QString uri = QtUtilities::htmlEscape(P2QSTRING(uriPart->target));
            result += "<a href='" + uri + "' >" + uri + "</a>";
            continue;
        }
        if ((emoticonPart = std::dynamic_pointer_cast<ChatWindow::ChatEmoticonMessagePart>(part))) {
            QString textStyle = showEmoticons_ ? "style='display:none'" : "";
            QString imageStyle = showEmoticons_ ? "" : "style='display:none'";
            result += "<span class='swift_emoticon_image' " + imageStyle + "><img src='" + P2QSTRING(emoticonPart->imagePath) + "'/></span><span class='swift_emoticon_text' " + textStyle + ">" + QtUtilities::htmlEscape(P2QSTRING(emoticonPart->alternativeText)) + "</span>";
            continue;
        }
        if ((highlightPart = std::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part))) {
            QString spanStart = getHighlightSpanStart(highlightPart->action.getFrontColor().get_value_or(""), highlightPart->action.getBackColor().get_value_or(""));
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
        std::shared_ptr<SecurityLabel> label,
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

    bool highlightWholeMessage = highlight.getFrontColor() || highlight.getBackColor();
    QString highlightSpanStart = highlightWholeMessage ? getHighlightSpanStart(highlight) : "";
    QString highlightSpanEnd = highlightWholeMessage ? "</span>" : "";
    htmlString += "<span class='swift_inner_message'>" + styleSpanStart + highlightSpanStart + message + highlightSpanEnd + styleSpanEnd + "</span>" ;

    bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasMessage, senderName, senderIsSelf);

    QString qAvatarPath = scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.svg" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
    std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(time), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id), direction));

    previousMessageWasSelf_ = senderIsSelf;
    previousSenderName_ = P2QSTRING(senderName);
    previousMessageKind_ = PreviousMessageWasMessage;
    return id;
}

std::string QtWebKitChatView::addAction(const ChatWindow::ChatMessage& message, const std::string &senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) {
    return addMessage(" *" + chatMessageToHTML(message) + "*", senderName, senderIsSelf, label, avatarPath, "font-style:italic ", time, message.getHighlightActionSender(), ChatSnippet::getDirection(message));
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

void QtWebKitChatView::resizeEvent(QResizeEvent* event) {
    // This code ensures that if the user is scrolled all to the bottom of a chat view,
    // the view stays scrolled to the bottom if the view is resized or if the message
    // input widget becomes multi line.
    if (isAtBottom_) {
        scrollToBottom();
    }
    QWidget::resizeEvent(event);
}

std::string QtWebKitChatView::addFileTransfer(const std::string& senderName, const std::string& avatarPath, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes, const std::string& description) {
    SWIFT_LOG(debug) << "addFileTransfer" << std::endl;
    QString ft_id = QString("ft%1").arg(P2QSTRING(boost::lexical_cast<std::string>(idCounter_++)));

    QString actionText;
    QString htmlString;
    QString formattedFileSize = P2QSTRING(formatSize(sizeInBytes));
    QString sanitizedFileName = QtUtilities::htmlEscape(P2QSTRING(filename));
    QString sanitizedDescription = QtUtilities::htmlEscape(P2QSTRING(description));
    if (senderIsSelf) {
        // outgoing
        filePaths_[ft_id] = sanitizedFileName;
        actionText = tr("Send file: %1 (%2)").arg(sanitizedFileName).arg(formattedFileSize);
        htmlString = actionText + " <br/>" +
            "<div id='" + ft_id + "'>" +
                buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
                buildChatWindowButton(tr("Set Description"), ButtonFileTransferSetDescription, ft_id) +
                buildChatWindowButton(tr("Send"), ButtonFileTransferSendRequest, ft_id) +
            "</div>";
    } else {
        // incoming
        actionText = tr("Receiving file: %1 (%2)").arg(sanitizedFileName).arg(formattedFileSize);
        if (!sanitizedDescription.isEmpty()) {
            actionText += QString(" \"%1\"").arg(sanitizedDescription);
        }
        htmlString = actionText + " <br/>" +
            "<div id='" + ft_id + "'>" +
                buildChatWindowButton(tr("Cancel"), ButtonFileTransferCancel, ft_id) +
                buildChatWindowButton(tr("Accept"), ButtonFileTransferAcceptRequest, ft_id, P2QSTRING(filename)) +
            "</div>";
    }

    bool appendToPrevious = appendToPreviousCheck(PreviousMessageWasFileTransfer, senderName, senderIsSelf);
    QString scaledAvatarPath = QtScaledAvatarCache(32).getScaledAvatarPath(avatarPath.c_str());
    QString qAvatarPath = scaledAvatarPath.isEmpty() ? "qrc:/icons/avatar.svg" : QUrl::fromLocalFile(scaledAvatarPath).toEncoded();
    std::string id = "ftmessage" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::universal_time()), qAvatarPath, senderIsSelf, appendToPrevious, theme_, P2QSTRING(id), ChatSnippet::getDirection(actionText)));

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
    QString qAvatarPath = "qrc:/icons/avatar.svg";
    std::string id = "wbmessage" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(contact), B2QDATE(boost::posix_time::second_clock::universal_time()), qAvatarPath, false, false, theme_, P2QSTRING(id), ChatSnippet::getDirection(actionText)));
    previousMessageWasSelf_ = false;
    previousSenderName_ = contact;
    return Q2PSTRING(wb_id);
}

void QtWebKitChatView::setWhiteboardSessionStatus(const std::string& id, const ChatWindow::WhiteboardSessionState state) {
    setWhiteboardSessionStatus(P2QSTRING(id), state);
}

static bool isFilePathWritable(const QString& path) {
    QFileInfo fileInfo = QFileInfo(path);
    if (fileInfo.exists()) {
        return fileInfo.isWritable();
    }
    else {
        bool writable = false;
        QFile writeTestFile(path);
        if (writeTestFile.open(QIODevice::ReadWrite)) {
            writeTestFile.write("test");
            if (writeTestFile.error() == QFileDevice::NoError) {
                writable = true;
            }
        }
        writeTestFile.close();
        writeTestFile.remove();
        return writable;
    }
}

void QtWebKitChatView::setFileTransferWarning(QString id, QString warningText) {
    QWebElement ftElement = findElementWithID(document_, "div", id);
    if (ftElement.isNull()) {
        SWIFT_LOG(debug) << "Tried to access FT UI via invalid id! id = " << Q2PSTRING(id) << std::endl;
        return;
    }

    removeFileTransferWarning(id);
    ftElement.appendInside(QString("<div class='ft_warning' style='color: red;'><br/>%1</div>").arg(QtUtilities::htmlEscape(warningText)));
}

void QtWebKitChatView::removeFileTransferWarning(QString id) {
    QWebElement ftElement = findElementWithID(document_, "div", id);
    if (ftElement.isNull()) {
        SWIFT_LOG(debug) << "Tried to access FT UI via invalid id! id = " << Q2PSTRING(id) << std::endl;
        return;
    }

    QWebElement warningElement = ftElement.findFirst(".ft_warning");
    if (!warningElement.isNull()) {
        warningElement.removeFromDocument();
    }
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
        if (!path.isEmpty() && isFilePathWritable(path)) {
            filePaths_[ft_id] = path;
            window_->onFileTransferAccept(Q2PSTRING(ft_id), Q2PSTRING(path));
            removeFileTransferWarning(ft_id);
        }
        else {
            setFileTransferWarning(ft_id, tr("The chosen save location is not writable! Click the 'Accept' button to select a different save location."));
        }
    }
    else if (id.startsWith(ButtonFileTransferOpenFile)) {
        QString ft_id = arg1;
        QString filename = arg2;
        askDesktopToOpenFile(filename);
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
        eventStream_->send(std::make_shared<JoinMUCUIEvent>(Q2PSTRING(roomJID), Q2PSTRING(password), boost::optional<std::string>(), false, false, isImpromptu.contains("true"), isContinuation.contains("true")));
        setMUCInvitationJoined(elementID);
    }
    else {
        SWIFT_LOG(debug) << "Unknown HTML button! ( " << Q2PSTRING(id) << " )" << std::endl;
    }
}

void QtWebKitChatView::handleVerticalScrollBarPositionChanged(double position) {
    rememberScrolledToBottom();
    if (position == 0) {
        emit scrollReachedTop();
    }
    else if (position == 1) {
        emit scrollReachedBottom();
    }
}

void QtWebKitChatView::addErrorMessage(const ChatWindow::ChatMessage& errorMessage) {
    if (window_->isWidgetSelected()) {
        window_->onAllMessagesRead();
    }

    QString errorMessageHTML(chatMessageToHTML(errorMessage));
    std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<SystemMessageSnippet>("<span class=\"error\">" + errorMessageHTML + "</span>", QDateTime::currentDateTime(), false, theme_, P2QSTRING(id), ChatSnippet::getDirection(errorMessage)));

    previousMessageWasSelf_ = false;
    previousMessageKind_ = PreviousMessageWasSystem;
}

std::string QtWebKitChatView::addSystemMessage(const ChatWindow::ChatMessage& message, ChatWindow::Direction direction) {
    if (window_->isWidgetSelected()) {
        window_->onAllMessagesRead();
    }

    QString messageHTML = chatMessageToHTML(message);
    std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<SystemMessageSnippet>(messageHTML, QDateTime::currentDateTime(), false, theme_, P2QSTRING(id), getActualDirection(message, direction)));

    previousMessageKind_ = PreviousMessageWasSystem;
    return id;
}

void QtWebKitChatView::replaceWithAction(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) {
    replaceMessage(" *" + chatMessageToHTML(message) + "*", id, time, "font-style:italic ", message.getHighlightActionSender());
}

void QtWebKitChatView::replaceMessage(const ChatWindow::ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) {
    replaceMessage(chatMessageToHTML(message), id, time, "", message.getHighlightActionSender());
}

void QtWebKitChatView::replaceSystemMessage(const ChatWindow::ChatMessage& message, const std::string& id, ChatWindow::TimestampBehaviour timestampBehavior) {
    replaceSystemMessage(chatMessageToHTML(message), P2QSTRING(id), timestampBehavior);
}

void QtWebKitChatView::replaceSystemMessage(const QString& newMessage, const QString& id, const ChatWindow::TimestampBehaviour timestampBehaviour) {
    rememberScrolledToBottom();
    QWebElement message = document_.findFirst("#" + id);
    if (!message.isNull()) {
        QWebElement replaceContent = message.findFirst("span.swift_message");
        assert(!replaceContent.isNull());
        QString old = replaceContent.toOuterXml();
        replaceContent.setInnerXml(ChatSnippet::escape(newMessage));

        if (timestampBehaviour == ChatWindow::UpdateTimestamp) {
            QWebElement replace = message.findFirst("span.swift_time");
            assert(!replace.isNull());
            replace.setInnerXml(ChatSnippet::timeToEscapedString(QDateTime::currentDateTime()));
        }
    }
    else {
        qWarning() << "Trying to replace element with id " << id << " but it's not there.";
    }
}

void QtWebKitChatView::replaceMessage(const QString& message, const std::string& id, const boost::posix_time::ptime& time, const QString& style, const HighlightAction& highlight) {
    if (!id.empty()) {
        if (window_->isWidgetSelected()) {
            window_->onAllMessagesRead();
        }

        QString messageHTML(message);

        QString styleSpanStart = style == "" ? "" : "<span style=\"" + style + "\">";
        QString styleSpanEnd = style == "" ? "" : "</span>";
        QString highlightSpanStart = (highlight.getFrontColor() || highlight.getBackColor()) ? getHighlightSpanStart(highlight) : "";
        QString highlightSpanEnd = (highlight.getFrontColor() || highlight.getBackColor()) ? "</span>" : "";
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
    std::string id = "id" + boost::lexical_cast<std::string>(idCounter_++);
    addMessageBottom(std::make_shared<SystemMessageSnippet>(messageHTML, QDateTime::currentDateTime(), false, theme_, P2QSTRING(id), getActualDirection(message, direction)));

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

    QString qAvatarPath = "qrc:/icons/avatar.svg";

    addMessageBottom(std::make_shared<MessageSnippet>(htmlString, QtUtilities::htmlEscape(P2QSTRING(senderName)), B2QDATE(boost::posix_time::second_clock::universal_time()), qAvatarPath, false, appendToPrevious, theme_, id, ChatSnippet::getDirection(message)));
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
            xml = "<img src='qrc:/icons/delivery-success.svg' title='" + tr("The receipt for this message has been received.") + "'/>";
            break;
        case ChatWindow::ReceiptRequested:
            xml = "<img src='qrc:/icons/delivery-warning.svg' title='" + tr("The receipt for this message has not yet been received. The recipient(s) might not have received this message.") + "'/>";
            break;
        case ChatWindow::ReceiptFailed:
            xml = "<img src='qrc:/icons/delivery-failure.svg' title='" + tr("Failed to transmit message to the receipient(s).") + "'/>";
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

}
