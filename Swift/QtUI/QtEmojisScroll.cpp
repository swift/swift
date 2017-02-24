/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmojisScroll.h>

#include <QLayout>
#include <QScrollArea>
#include <QStyle>

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
    }
}
