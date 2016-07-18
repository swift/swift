/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QGridLayout>
#include <QWidget>

namespace Swift {
    class QtEmojisScroll : public QWidget {
        Q_OBJECT
    public:
        QtEmojisScroll(QGridLayout* emojiLayout, QWidget *parent = 0);
    };
}
