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
#include <Swift/QtUI/Roster/GroupItemDelegate.h>

namespace Swift {
    class QtTreeWidget;
    class RosterDelegate : public QStyledItemDelegate {
    public:
        RosterDelegate(QtTreeWidget* tree, bool compact);
        ~RosterDelegate();
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    public slots:
        void setCompact(bool compact);
    private:
        QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paintGroup(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        bool compact_;
        DelegateCommons common_;
        GroupItemDelegate* groupDelegate_;
        QtTreeWidget* tree_;
    };
}
