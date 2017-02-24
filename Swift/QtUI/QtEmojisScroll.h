/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QLayout>
#include <QWidget>

namespace Swift {
    class QtEmojisScroll : public QWidget {
        Q_OBJECT
    public:
        QtEmojisScroll(QLayout* emojiLayout, QWidget *parent = 0);
    };
}
