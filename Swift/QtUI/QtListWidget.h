/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QListWidget>

namespace Swift {

class QtListWidget : public QListWidget {
    Q_OBJECT
    public:
        QtListWidget(QWidget* parent = nullptr);
    protected:
        virtual void resizeEvent(QResizeEvent*);
    signals:
        void onResize();
};

}
