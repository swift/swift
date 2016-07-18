/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtRecentEmojisGrid.h>

#include <algorithm>

#include <QSettings>
#include <QVector>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojiCell.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
    const int QtRecentEmojisGrid::MAX_RECENTS = 50;

    QtRecentEmojisGrid::QtRecentEmojisGrid(QSettings* settings) : QtEmojisGrid(), settings_(settings) {
        loadSettings();
        connect(this, SIGNAL(onEmojiSelected(QString)), this, SLOT(handleEmojiClicked(QString)));
        refresh();
    }

    QtRecentEmojisGrid::~QtRecentEmojisGrid() {
        writeSettings();
    }

    void QtRecentEmojisGrid::handleEmojiClicked(QString emoji) {
        recents_.erase(std::remove(recents_.begin(), recents_.end(), emoji), recents_.end());

        if (recents_.size() > MAX_RECENTS) {
            recents_.resize(MAX_RECENTS - 1);
        }

        recents_.push_front(emoji);
        refresh();
    }

    void QtRecentEmojisGrid::refresh() {
        QtEmojisGrid::setEmojis(recents_);
    }

    void QtRecentEmojisGrid::loadSettings() {
        QByteArray readData = settings_->value("recentEmojis").toByteArray();
        QDataStream readStream(&readData, QIODevice::ReadOnly);
        readStream >> recents_;
    }

    void QtRecentEmojisGrid::writeSettings() {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << recents_;
        settings_->setValue("recentEmojis", data);
    }
}
