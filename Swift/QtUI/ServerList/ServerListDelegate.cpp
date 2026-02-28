/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ServerList/ServerListDelegate.h>

#include <QApplication>
#include <QBitmap>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygon>

#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <Swift/QtUI/ServerList/ServerListModel.h>

namespace Swift {

ServerListDelegate::ServerListDelegate() {

}

ServerListDelegate::~ServerListDelegate() {

}

void ServerListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QColor bgColor(38, 81, 112);
    painter->fillRect(option.rect, bgColor);
    SwiftAccountData::SwiftAccountDataItem* item = static_cast<SwiftAccountData::SwiftAccountDataItem*>(index.internalPointer());
    paintServerAvatar(painter, option, item->iconPath_, item->status_, false, item->unreadCount_);
}

QSize ServerListDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
    //TODO Make this configurable.
    return QSize(75, 75);
}

void ServerListDelegate::paintServerAvatar(QPainter* painter, const QStyleOptionViewItem& option, const QString& avatarPath, const StatusShow& /*serverPresence*/, bool isIdle, size_t unreadCount) const {
    painter->save();
    QRect fullRegion(option.rect);
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(fullRegion, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    }
    auto secondLineColor = painter->pen().color();
    secondLineColor.setAlphaF(0.7);

    QRect presenceRegion(QPoint(common_.farLeftMargin, fullRegion.top() + common_.horizontalMargin), QSize(presenceIconWidth, presenceIconHeight));
    QRect idleIconRegion(QPoint(common_.farLeftMargin, fullRegion.top()), QSize(presenceIconWidth * 2, presenceIconHeight - common_.verticalMargin));
    int calculatedAvatarSize = fullRegion.height() - common_.verticalMargin;
    //This overlaps the presenceIcon, so must be painted first
    QRect avatarRegion(QPoint(presenceRegion.right() - common_.presenceIconWidth / 2, presenceRegion.top()), QSize(calculatedAvatarSize, calculatedAvatarSize));

    QPixmap avatarPixmap;
    if (!avatarPath.isEmpty()) {
        QString scaledAvatarPath = QtScaledAvatarCache(avatarRegion.height()).getScaledAvatarPath(avatarPath);
        if (QFileInfo(scaledAvatarPath).exists()) {
            avatarPixmap.load(scaledAvatarPath);
        }
    }
    if (avatarPixmap.isNull()) {
        avatarPixmap = QPixmap(":/icons/avatar.svg").scaled(avatarRegion.height(), avatarRegion.width(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    painter->drawPixmap(avatarRegion.topLeft() + QPoint(((avatarRegion.width() - avatarPixmap.width()) / 2), (avatarRegion.height() - avatarPixmap.height()) / 2), avatarPixmap);
    //Paint the presence status over the top of the avatar
    //FIXME enable drawing status when ServerPresence data are available.
    /*{
        //TODO make the colors consistent with chattables work from QtChatOverviewDelegate::paint, copying for now
        const auto green = QColor(124, 243, 145);
        const auto yellow = QColor(243, 243, 0);
        const auto red = QColor(255, 45, 71);
        const auto grey = QColor(159, 159, 159);
        QColor color = grey;
        switch (serverPresence.getType()) {
            case StatusShow::Online: color = green; break;
            case StatusShow::FFC: color = green; break;
            case StatusShow::Away: color = yellow; break;
            case StatusShow::XA: color = yellow; break;
            case StatusShow::DND: color = red; break;
            case StatusShow::None: color = grey; break;
        }
        QPen pen(color);
        pen.setWidth(1);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(pen);
        painter->setBrush(QBrush(color, Qt::SolidPattern));
        painter->drawEllipse(presenceRegion);
    }*/

    if (isIdle) {
        common_.idleIcon.paint(painter, idleIconRegion, Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (unreadCount > 0) {
        QRect unreadRect(avatarRegion.right() - common_.unreadCountSize - common_.horizontalMargin, avatarRegion.top(), common_.unreadCountSize, common_.unreadCountSize);
        QPen pen(QColor("black"));
        pen.setWidth(1);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor("red"), Qt::SolidPattern));
        painter->drawEllipse(unreadRect);
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->setPen(QColor("white"));
        common_.drawElidedText(painter, unreadRect, QString("%1").arg(unreadCount), Qt::AlignCenter);
    }
    painter->restore();
}

}
