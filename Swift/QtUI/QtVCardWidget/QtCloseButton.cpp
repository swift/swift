/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtCloseButton.h>

#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

namespace Swift {

QtCloseButton::QtCloseButton(QWidget *parent) : QAbstractButton(parent) {

}

QSize QtCloseButton::sizeHint() const {
    return QSize(style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, nullptr, nullptr), style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, nullptr, nullptr));
}

bool QtCloseButton::event(QEvent *e) {
    if (e->type() == QEvent::Enter || e->type() == QEvent::Leave) {
        update();
    }
    return QAbstractButton::event(e);
}

void QtCloseButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    QStyleOption opt;
    opt.init(this);
    opt.state |= QStyle::State_AutoRaise;
    if (underMouse() && !isDown()) {
        opt.state |= QStyle::State_Raised;
    } else if (isDown()) {
        opt.state |= QStyle::State_Sunken;
    }
    style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &painter, this);
}

}
