/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {

class ContactListDelegate : public QStyledItemDelegate {
    public:
        ContactListDelegate(bool compact);
        virtual ~ContactListDelegate();

        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    public slots:
        void setCompact(bool compact);

    private:
        bool compact_;
        DelegateCommons common_;
};
}
