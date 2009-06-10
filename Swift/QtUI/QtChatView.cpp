#include "QtChatView.h"

#include <QtDebug>
#include <QFile>
#include <QVBoxLayout>
#include <QWebView>
#include <QWebFrame>
#include <QKeyEvent>

namespace Swift {

QtChatView::QtChatView(QWidget* parent) : QWidget(parent) {
	setFocusPolicy(Qt::NoFocus);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);

	webView_ = new QWebView(this);
	webView_->setFocusPolicy(Qt::NoFocus);
	mainLayout->addWidget(webView_);

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
	webPage_->mainFrame()->setHtml(pageHTML);
}

void QtChatView::addMessage(const ChatSnippet& snippet) {
	//bool wasScrolledToBottom = isScrolledToBottom();
	
	QString content = snippet.getContent();
	content.replace("\\", "\\\\");
	content.replace("\"", "\\\"");
	content.replace("\n", "\\n");
	content.replace("\r", "");
	if (previousContinuationElementID_.isEmpty() || !snippet.getAppendToPrevious()) {
		webPage_->mainFrame()->evaluateJavaScript("appendMessage(\"" + content + "\");");
	}
	else {
		webPage_->mainFrame()->evaluateJavaScript("appendNextMessage(\"" + content + "\");");
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

}
