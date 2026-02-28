/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QLabel>

namespace Swift {
    class QtClickableLabel : public QLabel {
            Q_OBJECT
        public:
            QtClickableLabel(QWidget* parent);

            void mousePressEvent(QMouseEvent* event);

        signals:
            void clicked();
    };
}
