/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>

namespace Swift {

class QtSoundSelectionStyledItemDelegate : public QStyledItemDelegate {
    public:
        QtSoundSelectionStyledItemDelegate(QObject* parent = nullptr);

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex&) const;

    protected:
        virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
};

}
