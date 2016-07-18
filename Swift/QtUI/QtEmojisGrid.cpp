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
QtEmojisGrid::QtEmojisGrid() {

}

QtEmojisGrid::QtEmojisGrid(QString categoryName) {
        auto category = EmojiMapper::categoryNameToEmojis(Q2PSTRING(categoryName));

        QVector<QString> categoryEmojis;

        for (const auto& emoji : category) {
            categoryEmojis.push_back(P2QSTRING(emoji));
        }

        setEmojis(categoryEmojis);
    }

    void QtEmojisGrid::setEmojis(const QVector<QString>& emojis) {
        clearEmojis();

        int iEmoji = 0;
        for (const auto& unicodeEmoji : emojis) {
            QString shortname = QString::fromStdString(EmojiMapper::unicodeToShortname(Q2PSTRING(unicodeEmoji)));
            QtEmojiCell* emoji = new QtEmojiCell(shortname, unicodeEmoji);
            this->addWidget(emoji, iEmoji/6, iEmoji%6);
            connect(emoji, SIGNAL(emojiClicked(QString)), this, SIGNAL(onEmojiSelected(QString)));
            iEmoji++;
        }
        for (int index = 0; index < columnCount(); index++) {
            auto layoutItem = itemAtPosition(0, index);
            if (layoutItem) {
                auto cellWidget = layoutItem->widget();
                if (cellWidget) {
                    setColumnMinimumWidth(index, cellWidget->width());
                }
            }
        }
        setSpacing(5);
    }

    void QtEmojisGrid::clearEmojis() {
        QLayoutItem* child = nullptr;
        while ((child = this->takeAt(0)) != 0) {
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
