/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>

namespace Swift {

class QtCheckBoxStyledItemDelegate : public QStyledItemDelegate {
    public:
        QtCheckBoxStyledItemDelegate(QObject* parent = nullptr);

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex&) const;

    public:
        static const int DATA_ROLE = Qt::UserRole + 1;

    protected:
        virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
};

}
