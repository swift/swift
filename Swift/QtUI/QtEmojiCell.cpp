/*
 * Copyright (c) 2016-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojiCell.h>

#include <Swiften/Base/Platform.h>

#include <QFont>
#include <QFontMetrics>
#include <QPushButton>
#include <QString>

#include <SwifTools/EmojiMapper.h>

namespace Swift {
    QtEmojiCell::QtEmojiCell(QString shortname, QString text, QWidget* parent) :  QPushButton(parent) {
        setText(text);
        QFont font = this->font();
#ifdef SWIFTEN_PLATFORM_WINDOWS
        //Windows emoji font miscalculates the bounding rectangular that surrounds the emoji. We set a multiplier value to make it look consistent with linux & Mac
        const float sizeMultiplier = 1.3;
        font.setPointSize(18);
#else
        font.setPointSize(22);
        const float sizeMultiplier = 1;
#endif
        font.setBold(true);
        setFont(font);

        const auto boundingRect = fontMetrics().boundingRect("\xF0\x9F\x98\x83");
        setFixedWidth(qMax(sizeMultiplier*boundingRect.width(), sizeMultiplier*boundingRect.height()));
        setFixedHeight(qMax(sizeMultiplier*boundingRect.width(), sizeMultiplier*boundingRect.height()));

        setFlat(true);
        setToolTip(shortname);
        connect(this, SIGNAL(clicked()), this, SLOT(handleEmojiClicked()));
    }

    QtEmojiCell::QtEmojiCell(QIcon icon, QString text, QWidget* parent) : QPushButton(parent), text_(text) {
        setIcon(icon);
        setFlat(true);
        connect(this, SIGNAL(clicked()), this, SLOT(handleEmojiClicked()));
        setFixedSize(icon.availableSizes()[0] * 1.5);
    }

    QtEmojiCell::QtEmojiCell(const QtEmojiCell& b) : QtEmojiCell(b.toolTip(), b.text()) {

    }

    QtEmojiCell::~QtEmojiCell() {
    }

    void QtEmojiCell::handleEmojiClicked () {
        if (text_.isEmpty()) {
            emit emojiClicked(text());
        }
        else {
            emit emojiClicked(text_);
        }
    }

}
