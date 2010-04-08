/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QtDebug>
#include <QApplication>
#include <iostream>
#include <QWidget>
#include <QFile>
#include <QDateTime>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWebView>
#include <QWebFrame>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "../QtChatView.h"
#include "../MessageSnippet.h"
#include "../SystemMessageSnippet.h"

using namespace Swift;

/*
class MyNetworkReply : public QNetworkReply {
	public:
		MyNetworkReply() {
		}
		
		qint64 readData(char*, qint64) {
			return 0;
		}

		virtual void abort() {
		}
};

class MyNetworkAccessManager : public QNetworkAccessManager {
	public:
		MyNetworkAccessManager() {
		}

		QNetworkReply * createRequest (Operation op, const QNetworkRequest& request, QIODevice* outgoingData = 0) {
			assert(op == QNetworkAccessManager::GetOperation);
			qDebug() << "Requesting: " << request.url();
			return QNetworkAccessManager::createRequest(op, request, outgoingData);
			//return new MyNetworkReply();
		}
};

			QVBoxLayout* mainLayout = new QVBoxLayout(this);
			webView_ = new QWebView(this);

			QFile file(":/themes/Stockholm/Contents/Resources/Incoming/Content.html");
			file.open(QIODevice::ReadOnly);
			QString content = QString::fromUtf8(file.readAll());

			webPage_ = new QWebPage(this);
			webPage_->setNetworkAccessManager(new MyNetworkAccessManager());
			webView_->setPage(webPage_);
			QString pagehtml = 
				"<head>"
					//"<base href=\"file:///Users/remko/src/swift/resources/themes/Stockholm/Contents/Resources/\"/>"
					"<base href=\"file:///Users/remko/src/swift/resources/themes/Stockholm/Contents/Resources/\"/>"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"main.css\"/>"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"Variants/Alt Blue - Blue.css\"/>"
				"</head><body>" + content + "</body>";
			qDebug() << pagehtml;
			webPage_->mainFrame()->setHtml(pagehtml);

*/

/*

class ChatView : public QWidget {
	public:
		ChatView(QWidget* parent) : QWidget(parent) {
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

			QString pageHTML = "<head></head><body><div id=\"chat\"></div></body>";
			webPage_->mainFrame()->setHtml(pageHTML);
		}

		void appendHTML(const QString& html) {
			webPage_->mainFrame()->evaluateJavaScript(
				"newNode = document.createElement(\"div\");"
				"newNode.innerHTML = \"" + html + "\";" 
				"chatElement = document.getElementById(\"chat\");"
				"chatElement.appendChild(newNode);");
			webPage_->mainFrame()->setScrollBarValue(Qt::Vertical, webPage_->mainFrame()->scrollBarMaximum(Qt::Vertical));
		}

	private:
		QWebView* webView_;
		QWebPage* webPage_;
};
*/

class MyWidget : public QWidget {
		Q_OBJECT

	public:
		MyWidget() : previousWasIncoming_(false), previousWasOutgoing_(false), previousWasSystem_(false) {
			QVBoxLayout* mainLayout = new QVBoxLayout(this);
			chatView_ = new QtChatView(this);
			mainLayout->addWidget(chatView_);
			input1_ = new QLineEdit(this);
			connect(input1_, SIGNAL(returnPressed()), SLOT(addIncoming()));
			mainLayout->addWidget(input1_);
			input2_ = new QLineEdit(this);
			connect(input2_, SIGNAL(returnPressed()), SLOT(addOutgoing()));
			mainLayout->addWidget(input2_);
			input3_ = new QLineEdit(this);
			connect(input3_, SIGNAL(returnPressed()), SLOT(addSystem()));
			mainLayout->addWidget(input3_);

			resize(300,200);
		}
	
	public slots:
		void addIncoming() {
			chatView_->addMessage(MessageSnippet(input1_->text(), "Me", QDateTime::currentDateTime(), "qrc:/icons/avatar.png", true, previousWasIncoming_));
			previousWasIncoming_ = true;
			previousWasOutgoing_ = false;
			previousWasSystem_ = false;
			input1_->clear();
		}

		void addOutgoing() {
			chatView_->addMessage(MessageSnippet(input2_->text(), "You", QDateTime::currentDateTime(), "qrc:/icons/avatar.png", false, previousWasOutgoing_));
			previousWasIncoming_ = false;
			previousWasOutgoing_ = true;
			previousWasSystem_ = false;
			input2_->clear();
		}

		void addSystem() {
			chatView_->addMessage(SystemMessageSnippet(input3_->text(), QDateTime::currentDateTime(), previousWasSystem_));
			previousWasIncoming_ = false;
			previousWasOutgoing_ = false;
			previousWasSystem_ = true;
			input3_->clear();
		}

	private:
		bool previousWasIncoming_;
		bool previousWasOutgoing_;
		bool previousWasSystem_;
		QtChatView* chatView_;
		QLineEdit* input1_;
		QLineEdit* input2_;
		QLineEdit* input3_;
};
		

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	MyWidget w;
	w.show();
	return app.exec();
}

#include "main.moc"
