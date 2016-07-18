/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QGridLayout>
#include <QString>
#include <QVector>

#include <SwifTools/EmojiMapper.h>

namespace Swift {
    class QtEmojisGrid : public QGridLayout {
        Q_OBJECT
        public:
            explicit QtEmojisGrid();
            explicit QtEmojisGrid(QString categoryName);

        protected:
            void setEmojis(const QVector<QString>& emojis);

        private:
            void clearEmojis();

        signals:
            void onEmojiSelected(QString emoji);
    };
}
