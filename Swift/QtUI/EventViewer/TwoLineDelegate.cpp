/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/EventViewer/TwoLineDelegate.h>

#include <QColor>
#include <QPainter>
#include <QPen>

namespace Swift {

namespace {
    const QColor secondLineColor = QColor(160,160,160);
}

TwoLineDelegate::TwoLineDelegate(int firstRole, int secondRole, bool wrap) {
    firstRole_ = firstRole;
    secondRole_ = secondRole;
    wrap_ = wrap;
}

TwoLineDelegate::~TwoLineDelegate() {

}

QSize TwoLineDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, QtEvent* /*event*/ ) const {
    QFontMetrics nameMetrics(common_.nameFont);
    QFontMetrics statusMetrics(common_.detailFont);
    int sizeByText = 2 * common_.verticalMargin + nameMetrics.height() + statusMetrics.height();
    return QSize(150, sizeByText);
}

void TwoLineDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, QtEvent* event) const {
    painter->save();
    QRect fullRegion(option.rect);
    if ( option.state & QStyle::State_Selected ) {
        painter->fillRect(fullRegion, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    } else {
        QColor nameColor = event->data(Qt::TextColorRole).value<QColor>();
        painter->setPen(QPen(nameColor));
    }

    QFontMetrics nameMetrics(common_.nameFont);
    painter->setFont(common_.nameFont);
    int extraFontWidth = nameMetrics.width("H");
    int leftOffset = common_.horizontalMargin * 2 + extraFontWidth / 2;
    QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0, 0));

    int nameHeight = nameMetrics.height() + common_.verticalMargin;
    QRect nameRegion(textRegion.adjusted(0, common_.verticalMargin, 0, 0));

    DelegateCommons::drawElidedText(painter, nameRegion, event->data(firstRole_).toString());

    painter->setFont(common_.detailFont);
    painter->setPen(QPen(secondLineColor));

    QRect detailRegion(textRegion.adjusted(0, nameHeight, 0, 0));
    DelegateCommons::drawElidedText(painter, detailRegion, event->data(secondRole_).toString());

    painter->restore();
}

}
