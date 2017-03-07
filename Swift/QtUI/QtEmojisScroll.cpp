/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojisScroll.h>

#include <QLayout>
#include <QScrollArea>
#include <QStyle>

#include <Swiften/Base/Platform.h>

#include <Swift/QtUI/QtEmojisGrid.h>
#include <Swift/QtUI/QtRecentEmojisGrid.h>

namespace Swift {
    QtEmojisScroll::QtEmojisScroll(QLayout* emojiLayout, QWidget *parent) : QWidget(parent) {
        auto selector = new QWidget();
        auto scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(selector);
        selector->setLayout(emojiLayout);

        this->setLayout(new QVBoxLayout);
        this->layout()->addWidget(scrollArea);
        this->layout()->setContentsMargins(0,0,0,0);

        if (emojiLayout->itemAt(0)) {
#ifdef SWIFTEN_PLATFORM_MACOSX
            setMinimumHeight(emojiLayout->itemAt(0)->minimumSize().height() * 8);
#else
            setMinimumHeight(emojiLayout->itemAt(0)->minimumSize().height() * 2);
#endif
        }
    }
}
