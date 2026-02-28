/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {
    class UserSearchDelegate : public QStyledItemDelegate {
        Q_OBJECT

        public:
            UserSearchDelegate(QObject* parent = nullptr);
            virtual ~UserSearchDelegate();

            void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
            QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const;
        private:
            DelegateCommons common_;
    };

}

