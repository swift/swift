/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>

namespace Swift {
    class ColorWidget : public QWidget {
        Q_OBJECT
    public:
        ColorWidget(QWidget* parent = nullptr);
        QSize sizeHint() const;

    public slots:
        void setColor(QColor color);

    private:
        QColor color;

    protected:
        void paintEvent(QPaintEvent* /*event*/);
        void mouseReleaseEvent(QMouseEvent* event);

    signals:
        void clicked();

    };
}

