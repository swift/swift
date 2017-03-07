/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QPushButton>
#include <QString>

#include <SwifTools/EmojiMapper.h>

class QWidget;
class QMouseEvent;

namespace Swift {
    class QtEmojiCell : public QPushButton {
        Q_OBJECT
        public:
            QtEmojiCell(const QtEmojiCell& b);
            QtEmojiCell(QString shortname, QString text, QWidget* parent = nullptr);
            QtEmojiCell(QIcon icon, QString text, QWidget* parent = nullptr);
            ~QtEmojiCell();

        signals:
            void emojiClicked(QString emojiAsText);

        private slots:
            void handleEmojiClicked();

        private:
            QString text_;
    };
}
