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
    QtEmojisScroll::QtEmojisScroll(QGridLayout* emojiLayout, QWidget *parent) : QWidget(parent) {
        auto selector = new QWidget();
        auto scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(selector);
        // Set minimum width to fit GridLayout (no horizontal ScrollBar)
        const int margin = style()->pixelMetric(QStyle::PM_MenuHMargin) * 2;
        scrollArea->setMinimumWidth((emojiLayout->columnCount()+1)*(emojiLayout->columnMinimumWidth(0)+emojiLayout->spacing())+margin);
        // Set height according to width (better ratio)
        const double ratio = 16.0/9.0; //ratio width/height
        scrollArea->setMinimumHeight(scrollArea->minimumWidth()/ratio);
        selector->setLayout(emojiLayout);

        this->setLayout(new QVBoxLayout);
        this->layout()->addWidget(scrollArea);
        this->layout()->setContentsMargins(0,0,0,0);
    }
}
