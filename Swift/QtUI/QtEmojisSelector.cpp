/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojisSelector.h>

#include <QScrollArea>
#include <QSettings>
#include <QString>
#include <QTabBar>
#include <QWidget>

#include <Swiften/Base/Platform.h>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojisGrid.h>
#include <Swift/QtUI/QtEmojisScroll.h>
#include <Swift/QtUI/QtEmoticonsGrid.h>
#include <Swift/QtUI/QtRecentEmojisGrid.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
    QtEmojisSelector::QtEmojisSelector(QSettings* settings, const std::map<std::string, std::string>& emoticonsMap, QWidget* parent) : QTabWidget(parent), settings_(settings), emoticonsMap_(emoticonsMap) {
#ifdef SWIFTEN_PLATFORM_MACOSX
        recentEmojisGrid_ = addRecentTab();
        connect(recentEmojisGrid_, SIGNAL(onEmojiSelected(QString)), this, SLOT(emojiClickedSlot(QString)));

        for (const auto& category : EmojiMapper::getCategories()) {
            if (category != "modifier") {
                QtEmojisGrid* grid = addTab(P2QSTRING(category));
                connect(grid, SIGNAL(onEmojiSelected(QString)), this, SLOT(emojiClickedSlot(QString)));
            }
        }

        loadSettings();
#else
        setupEmoticonsTab();
#endif
    }

    QtEmojisSelector::~QtEmojisSelector() {
#ifdef SWIFTEN_PLATFORM_MACOSX
        writeSettings();
#endif
    }

    QtRecentEmojisGrid* QtEmojisSelector::addRecentTab() {
        QtRecentEmojisGrid* recent = new QtRecentEmojisGrid(settings_);
        QtEmojisScroll* scroll = new QtEmojisScroll(recent);
        QTabWidget::addTab(scroll, QString::fromStdString(EmojiMapper::categoryToFlagshipUnicodeEmoji("recent")));

        setTabToolTip(count()-1, tr("Recent"));

        return recent;
    }

    QtEmojisGrid* QtEmojisSelector::addTab(QString categoryName) {
        QtEmojisGrid* grid = new QtEmojisGrid(categoryName);
        QtEmojisScroll* scroll = new QtEmojisScroll(grid);
        QTabWidget::addTab(scroll, QString::fromStdString(EmojiMapper::categoryToFlagshipUnicodeEmoji(Q2PSTRING(categoryName))));
        setTabToolTip(count()-1, categoryName.replace(0, 1, categoryName[0].toUpper()));

        return grid;
    }

    void QtEmojisSelector::loadSettings() {
        if (settings_->contains("currentEmojiTab")) {
            setCurrentIndex(settings_->value("currentEmojiTab").toInt());
        } else {
            setCurrentIndex(1); //index of people category
        }
    }

    void QtEmojisSelector::writeSettings() {
        settings_->setValue("currentEmojiTab", currentIndex());
    }

    void QtEmojisSelector::setupEmoticonsTab() {
        QtEmojisGrid* grid = new QtEmoticonsGrid(emoticonsMap_);
        QtEmojisScroll* scroll = new QtEmojisScroll(grid);
        QTabWidget::addTab(scroll, QIcon(":/emoticons/smile.png"),"");
        connect(grid, SIGNAL(onEmojiSelected(QString)), this, SLOT(emojiClickedSlot(QString)));
        tabBar()->hide();
    }

    void QtEmojisSelector::emojiClickedSlot(QString emoji) {
        if (recentEmojisGrid_) {
            recentEmojisGrid_->handleEmojiClicked(emoji);
        }
        emit emojiClicked(emoji);
    }
}
