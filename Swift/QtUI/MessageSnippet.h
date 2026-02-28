/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QString>

#include <Swift/QtUI/ChatSnippet.h>

class QDateTime;

namespace Swift {
    class MessageSnippet : public ChatSnippet {
        public:
            MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious, QtChatTheme* theme, const QString& id, Direction direction);
            virtual ~MessageSnippet();
            const QString& getContent() const {
                return content_;
            }

            QString getContinuationElementID() const {
                return "insert";
            }

        private:
            QString content_;
    };
}
