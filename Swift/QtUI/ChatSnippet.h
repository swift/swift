/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QDateTime>
#include <QString>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

#include <Swift/QtUI/QtChatTheme.h>

namespace Swift {
    class ChatSnippet {
        public:
            enum Direction {
                RTL,
                LTR
            };

            ChatSnippet(bool appendToPrevious);
            virtual ~ChatSnippet();

            virtual const QString& getContent() const = 0;
            virtual QString getContinuationElementID() const { return ""; }

            std::shared_ptr<ChatSnippet> getContinuationFallbackSnippet() const {return continuationFallback_;}

            bool getAppendToPrevious() const {
                return appendToPrevious_;
            }

            static QString escape(const QString& original) {
                QString result(original);
                result.replace("%message%", "&#37;message&#37;");
                result.replace("%sender%", "&#37;sender&#37;");
                result.replace("%wrapped_sender%", "&#37;wrapped_sender&#37;");
                result.replace("%time%", "%&#37;time&#37;");
                result.replace("%shortTime%", "%&#37;shortTime&#37;");
                result.replace("%userIconPath%", "&#37;userIconPath&#37;");
                result.replace("%id%", "&#37;id&#37;");
                result.replace("\t", "    ");
                result.replace("  ", "&nbsp;&nbsp;");
                return result;
            }

            static QString timeToEscapedString(const QDateTime& time);

            static Direction getDirection(const std::string& message);
            static Direction getDirection(const ChatWindow::ChatMessage& message);
            static Direction getDirection(const QString& message);

        protected:
            static QString directionToCSS(Direction direction);

            QString wrapResizable(const QString& text);
            void setContinuationFallbackSnippet(std::shared_ptr<ChatSnippet> continuationFallback) {
                continuationFallback_ = continuationFallback;
            }
        private:
            bool appendToPrevious_;
            std::shared_ptr<ChatSnippet> continuationFallback_;
    };
}

