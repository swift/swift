/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QTabBar>

#include <Swift/QtUI/QtTabWidget.h>

namespace Swift {

class QtDNDTabBar : public QTabBar {
    Q_OBJECT
    public:
        explicit QtDNDTabBar(QWidget* parent = nullptr);
        virtual ~QtDNDTabBar();

        int getDragIndex() const;
        QString getDragText() const;
        QWidget* getDragWidget() const;

        virtual QSize sizeHint() const;

        friend class QtTabWidget;
    signals:
        void onDropSucceeded();

    protected:
        virtual void dragEnterEvent(QDragEnterEvent* dragEnterEvent);
        virtual void dropEvent(QDropEvent* dropEvent);
        virtual bool event(QEvent* event);
        virtual QSize tabSizeHint(int index) const;

    private:
        int defaultTabHeight;
        int dragIndex = -1;
        QString dragText;
        QWidget* dragWidget = nullptr;
};

}
