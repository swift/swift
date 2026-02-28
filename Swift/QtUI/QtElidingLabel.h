/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QLabel>

namespace Swift {
    class QtElidingLabel : public QLabel {
        Q_OBJECT
    public:
        QtElidingLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Widget);
        QtElidingLabel(const QString &text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Widget);
        void setText(const QString& text);
        virtual ~QtElidingLabel();

        virtual QSize sizeHint() const;

        virtual void paintEvent(QPaintEvent* event);

    private:
        void setSizes();
        bool dirty_;
        QString fullText_;
        QRect lastRect_;
        QSize sizeHint_;
    };
}
