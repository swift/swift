/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QGridLayout>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QVector>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojisGrid.h>

namespace Swift {
    class QtRecentEmojisGrid : public QtEmojisGrid {
        Q_OBJECT
    public:
        explicit QtRecentEmojisGrid(QSettings* settings);
        ~QtRecentEmojisGrid();

    public slots:
        void handleEmojiClicked(QString emoji);

    private:
        void refresh();
        void loadSettings();
        void writeSettings();

    private:
        static const int MAX_RECENTS;

    private:
        QVector<QString> recents_;
        QSettings* settings_;
    };
}
