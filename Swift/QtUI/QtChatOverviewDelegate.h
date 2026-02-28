/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QItemDelegate>
#include <QFont>

namespace Swift {
    class QtChatOverviewDelegate : public QItemDelegate {
            Q_OBJECT
        public:
            QtChatOverviewDelegate(QObject* parent);
            ~QtChatOverviewDelegate() override;
            QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
            void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        private:
            QFont nameFont;
    };
}
