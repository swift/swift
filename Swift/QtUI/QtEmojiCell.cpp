/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojiCell.h>

#include <QFont>
#include <QFontMetrics>
#include <QPushButton>
#include <QString>

#include <SwifTools/EmojiMapper.h>

namespace Swift {
    QtEmojiCell::QtEmojiCell(QString shortname, QString text, QWidget* parent) :  QPushButton(parent) {
        setText(text);
        QFont font = this->font();
        font.setPointSize(22);
        font.setBold(true);
        setFont(font);

        const auto boundingRect = fontMetrics().boundingRect("\xF0\x9F\x98\x83");
        setFixedWidth(qMax(boundingRect.width(), boundingRect.height()));
        setFixedHeight(qMax(boundingRect.width(), boundingRect.height()));

        setFlat(true);
        setToolTip(shortname);
        connect(this, SIGNAL(clicked()), this, SLOT(handleEmojiClicked()));
    }

    QtEmojiCell::QtEmojiCell(const QtEmojiCell& b) : QtEmojiCell(b.toolTip(), b.text()) {
    }

    QtEmojiCell::~QtEmojiCell() {
    }

    void QtEmojiCell::handleEmojiClicked () {
        emit emojiClicked(text());
    }

}
