/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtPlainChatView.h>

#include <QTextEdit>
#include <QVBoxLayout>

#include <Swiften/Base/foreach.h>

#include <Swift/QtUI/ChatSnippet.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>


namespace Swift {

QtPlainChatView::QtPlainChatView(QWidget* parent) : QtChatView(parent) {
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0,0,0,0);
	log_ = new QTextEdit(this);
	log_->setReadOnly(true);
	log_->setAccessibleName(tr("Chat Messages"));
	mainLayout->addWidget(log_);
}

QtPlainChatView::~QtPlainChatView() {
	
}

QString chatMessageToString(const ChatWindow::ChatMessage& message) {
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
			result += P2QSTRING(emoticonPart->alternativeText);
			continue;
		}
		if ((highlightPart = boost::dynamic_pointer_cast<ChatWindow::ChatHighlightingMessagePart>(part))) {
			//FIXME: Maybe do something here. Anything, really.
			continue;
		}
	}
	return result;
}

std::string QtPlainChatView::addMessage(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& /*avatarPath*/, const boost::posix_time::ptime& time, const HighlightAction& /*highlight*/) {
	QString text = "<p>";
	if (label) {
		text += P2QSTRING(label->getLabel()) + "<br/>";
	}
	QString name = senderIsSelf ? "you" : P2QSTRING(senderName);
	text += QString(tr("At %1 %2 said:")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name) + "<br/>";
	text += chatMessageToString(message);
	text += "</p>";
	log_->append(text);
	return "";
};

std::string QtPlainChatView::addAction(const ChatWindow::ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& /*avatarPath*/, const boost::posix_time::ptime& time, const HighlightAction& /*highlight*/) {
	QString text = "<p>";
	if (label) {
		text += P2QSTRING(label->getLabel()) + "<br/>";
	}
	QString name = senderIsSelf ? "you" : P2QSTRING(senderName);
	text += QString(tr("At %1 %2 ")).arg(ChatSnippet::timeToEscapedString(B2QDATE(time))).arg(name);
	text += chatMessageToString(message);
	text += "</p>";
	log_->append(text);
	return "";
};


}
