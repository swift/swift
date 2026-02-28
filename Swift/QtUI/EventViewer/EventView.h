/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QListView>

namespace Swift {
    class EventView : public QListView {
        Q_OBJECT
        public:
            EventView(QWidget* parent);
    };
}
