/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatOverviewDelegate.h>

#include <Swiften/Elements/StatusShow.h>

#include <Swift/QtUI/ChattablesModel.h>
#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {

QtChatOverviewDelegate::QtChatOverviewDelegate(QObject* parent) : QItemDelegate(parent), nameFont(QApplication::font()) {

}

QtChatOverviewDelegate::~QtChatOverviewDelegate() {}

QSize QtChatOverviewDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
    int heightByAvatar = DelegateCommons::avatarSize + DelegateCommons::verticalMargin * 2;
    QFontMetrics nameMetrics(nameFont);
    int sizeByText = 2 * DelegateCommons::verticalMargin + nameMetrics.height();
    return QSize(150, sizeByText > heightByAvatar ? sizeByText : heightByAvatar);
}

void QtChatOverviewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    QRect fullRegion(option.rect);
    const int statusCircleRadius = 3;
    const int horizontalMargin = 4;

    QColor bgColor(38, 81, 112);
    QPen fontPen("white"); // FIXME

    if (option.state & QStyle::State_Selected) {
        //FIXME
    }
    painter->fillRect(fullRegion, bgColor);
    painter->setPen(fontPen);

    QFontMetrics nameMetrics(nameFont);
    painter->setFont(nameFont);
    QRect nameRegion(fullRegion.adjusted((horizontalMargin + statusCircleRadius) * 2, DelegateCommons::verticalMargin, 0, 0));
    DelegateCommons::drawElidedText(painter, nameRegion, index.data(Qt::DisplayRole).toString());

    const auto green = QColor(124, 243, 145);
    const auto yellow = QColor(243, 243, 0);
    const auto red = QColor(255, 45, 71);
    const auto grey = QColor(159,159,159);
    auto circleColour = grey;
    auto status = static_cast<StatusShow::Type>(index.data(ChattablesModel::StatusRole).toInt());
    switch (status) {
        case StatusShow::Online: circleColour = green; break;
        case StatusShow::FFC: circleColour = green; break;
        case StatusShow::Away: circleColour = yellow; break;
        case StatusShow::XA: circleColour = yellow; break;
        case StatusShow::DND: circleColour = red; break;
        case StatusShow::None: circleColour = grey; break;
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    int unreadCount = index.data(ChattablesModel::UnreadCountRole).toInt();
    if (unreadCount > 0) {
        int unreadCountSize = 16;
        QRect unreadRect(fullRegion.right() - unreadCountSize - horizontalMargin, fullRegion.top() + (fullRegion.height() - unreadCountSize) / 2, unreadCountSize, unreadCountSize);
        QPen pen(QColor("white"));
        pen.setWidth(1);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(pen);
        painter->drawEllipse(unreadRect);
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->setPen(QColor("white"));
        DelegateCommons::drawElidedText(painter, unreadRect, QString("%1").arg(unreadCount), Qt::AlignCenter);
    }

    painter->setPen(circleColour);
    painter->setBrush(circleColour);
    painter->drawEllipse(fullRegion.topLeft() + QPointF(horizontalMargin + 4, fullRegion.height() / 2), statusCircleRadius, statusCircleRadius);


    painter->restore();
}

} // namespace Swift
