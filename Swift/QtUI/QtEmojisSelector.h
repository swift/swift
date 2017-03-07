/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <QSettings>
#include <QTabWidget>

#include <Swift/QtUI/QtEmojisGrid.h>
#include <Swift/QtUI/QtRecentEmojisGrid.h>

namespace Swift {

class QtEmojisSelector : public QTabWidget {
    Q_OBJECT
    public:
        QtEmojisSelector(QSettings* settings, const std::map<std::string, std::string>& emoticonsMap, QWidget * parent = 0);
        ~QtEmojisSelector();

    public slots:
        void emojiClickedSlot(QString emoji);

    signals:
        void emojiClicked(QString emoji);

    private:
        QtRecentEmojisGrid* addRecentTab();
        QtEmojisGrid* addTab(QString categoryName);
        void loadSettings();
        void writeSettings();

        void setupEmoticonsTab();

    private:
        QSettings* settings_ = nullptr;
        QtRecentEmojisGrid* recentEmojisGrid_ = nullptr;
        std::map<std::string, std::string> emoticonsMap_;
};

}
