/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QString>
#include <QVector>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/FlowLayout.h>

namespace Swift {
    class QtEmojisGrid : public FlowLayout {
        Q_OBJECT
        public:
            explicit QtEmojisGrid();
            explicit QtEmojisGrid(QString categoryName);

        protected:
            void setEmojis(const QVector<QString>& emojis);
            void addEmoticon(QIcon icon, QString text);

        private:
            void clearEmojis();

        signals:
            void onEmojiSelected(QString emoji);
    };
}
