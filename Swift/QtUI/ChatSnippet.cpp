/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatSnippet.h>

#include <QFile>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

ChatSnippet::ChatSnippet(bool appendToPrevious) : appendToPrevious_(appendToPrevious) {
}

ChatSnippet::~ChatSnippet() {
}

QString ChatSnippet::timeToEscapedString(const QDateTime& time) {
    QDate now(QDate::currentDate());
    QString date = "";
    if (time.date().daysTo(now) > 0) {
        date = "ddd ";
    }
    if (time.date().month() != now.month()) {
        date = date + "MMMM ";
    }
    if (time.date().daysTo(now) > 6) {
        date = date + "d ";
    }
    if (time.date().year() != now.year()) {
        date = date + "yy ";
    }
    date += "h:mm";
    return escape(time.toString(date));
}

QString ChatSnippet::wrapResizable(const QString& text) {
    return "<span class='swift_resizable'>" + text + "</span>";
}

QString ChatSnippet::directionToCSS(Direction direction) {
    return direction == RTL ? QString("rtl") : QString("ltr");
}

ChatSnippet::Direction ChatSnippet::getDirection(const ChatWindow::ChatMessage& message) {
    std::shared_ptr<ChatWindow::ChatTextMessagePart> textPart;
    std::string text = "";
    for (auto&& part : message.getParts()) {
        if ((textPart = std::dynamic_pointer_cast<ChatWindow::ChatTextMessagePart>(part))) {
            text = textPart->text;
            break;
        }
    }
    return getDirection(text);
}

ChatSnippet::Direction ChatSnippet::getDirection(const std::string& message) {
    return getDirection(P2QSTRING(message));
}

ChatSnippet::Direction ChatSnippet::getDirection(const QString& message) {
    /*
    for (int i = 0; i < message.size(); ++i) {
        switch (message.at(i).direction()) {
            case QChar::DirL:
            case QChar::DirLRE:
            case QChar::DirLRO:
                return ChatSnippet::LTR;
            case QChar::DirR:
            case QChar::DirAL:
            case QChar::DirRLE:
            case QChar::DirRLO:
                return ChatSnippet::RTL;
            case QChar::DirEN:
            case QChar::DirES:
            case QChar::DirET:
            case QChar::DirAN:
            case QChar::DirCS:
            case QChar::DirB:
            case QChar::DirWS:
            case QChar::DirON:
            case QChar::DirS:
            case QChar::DirPDF:
            case QChar::DirNSM:
            case QChar::DirBN:
                break;
        }
    }
    return ChatSnippet::LTR;
    */
    return message.isRightToLeft() ? ChatSnippet::RTL : ChatSnippet::LTR;
}


}
