/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QItemDelegate>

namespace Swift {

class QtRemovableItemDelegate : public QItemDelegate {
    public:
        QtRemovableItemDelegate(const QStyle* style);

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex&) const;
        virtual QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
        virtual QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;

    protected:
        virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

    private:
        const QStyle* style;
};

}
