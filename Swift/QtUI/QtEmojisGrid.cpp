/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojisGrid.h>

#include <string>

#include <QString>
#include <QVector>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojiCell.h>
#include <Swift/QtUI/QtRecentEmojisGrid.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
    static const int emojiCellSpacing = 2;

    QtEmojisGrid::QtEmojisGrid() : FlowLayout(0, emojiCellSpacing, emojiCellSpacing) {

    }

    QtEmojisGrid::QtEmojisGrid(QString categoryName) : FlowLayout(0, emojiCellSpacing, emojiCellSpacing) {
        auto category = EmojiMapper::categoryNameToEmojis(Q2PSTRING(categoryName));

        QVector<QString> categoryEmojis;

        for (const auto& emoji : category) {
            categoryEmojis.push_back(P2QSTRING(emoji));
        }

        setEmojis(categoryEmojis);
    }

    void QtEmojisGrid::setEmojis(const QVector<QString>& emojis) {
        clearEmojis();

        for (const auto& unicodeEmoji : emojis) {
            QString shortname = QString::fromStdString(EmojiMapper::unicodeToShortname(Q2PSTRING(unicodeEmoji)));
            auto emoji = new QtEmojiCell(shortname, unicodeEmoji);
            connect(emoji, SIGNAL(emojiClicked(QString)), this, SIGNAL(onEmojiSelected(QString)));
            addItem(new QWidgetItem(emoji));
        }
    }

    void QtEmojisGrid::addEmoticon(QIcon icon, QString text) {
        auto emoji = new QtEmojiCell(icon, text);
        connect(emoji, SIGNAL(emojiClicked(QString)), this, SIGNAL(onEmojiSelected(QString)));
        addItem(new QWidgetItem(emoji));
    }

    void QtEmojisGrid::clearEmojis() {
        QLayoutItem* child = nullptr;
        while ((child = this->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->hide();
                removeWidget(child->widget());
            }
            else {
                removeItem(child);
            }
        }
    }
}
