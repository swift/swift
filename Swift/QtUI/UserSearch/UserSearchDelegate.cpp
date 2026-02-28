/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/UserSearchDelegate.h>

#include <QFontMetrics>
#include <QModelIndex>
#include <QPainter>
#include <QPen>
#include <QStyleOptionViewItem>

#include <Swift/Controllers/Chat/UserSearchController.h>

#include <Swift/QtUI/UserSearch/UserSearchModel.h>

namespace Swift {

UserSearchDelegate::UserSearchDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

UserSearchDelegate::~UserSearchDelegate() {

}

QSize UserSearchDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const {
    //UserSearchItem* item = static_cast<UserSearchItem*>(index.internalPointer());
    QFontMetrics nameMetrics(common_.nameFont);
    QFontMetrics statusMetrics(common_.detailFont);
    int sizeByText = 2 * common_.verticalMargin + nameMetrics.height() + statusMetrics.height();
    return QSize(150, sizeByText);
}

void UserSearchDelegate::paint(QPainter* painter,    const QStyleOptionViewItem& option, const QModelIndex& index) const {
    UserSearchResult* item = static_cast<UserSearchResult*>(index.internalPointer());
    painter->save();
    QRect fullRegion(option.rect);
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(fullRegion, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    }
    else {
        QColor nameColor = UserSearchModel::data(item, Qt::TextColorRole).value<QColor> ();
        painter->setPen(QPen(nameColor));
    }

    QFontMetrics nameMetrics(common_.nameFont);
    painter->setFont(common_.nameFont);
    int extraFontWidth = nameMetrics.width("H");
    int leftOffset = common_.horizontalMargin * 2 + extraFontWidth / 2;
    QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0, 0));

    int nameHeight = nameMetrics.height() + common_.verticalMargin;
    QRect nameRegion(textRegion.adjusted(0, common_.verticalMargin, 0, 0));

    painter->drawText(nameRegion, Qt::AlignTop, UserSearchModel::data(item, Qt::DisplayRole).toString());

    painter->setFont(common_.detailFont);
    painter->setPen(QPen(QColor(160, 160, 160)));

    QRect detailRegion(textRegion.adjusted(0, nameHeight, 0, 0));
    painter->drawText(detailRegion, Qt::AlignTop, UserSearchModel::data(item, UserSearchModel::DetailTextRole).toString());

    painter->restore();
}

}
