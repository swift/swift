/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QColor>
#include <QFont>
#include <QStyledItemDelegate>

#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {
    class QtTreeWidgetItem;
    class GroupItemDelegate {
    public:
        GroupItemDelegate();
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QString& name, int rowCount, bool expanded) const;
    private:
        void paintShadowText(QPainter* painter, const QRect& region, const QString& text) const;
        void paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const;
        QFont groupFont_;
        static const int groupCornerRadius_;
        DelegateCommons common_;
    };
}
