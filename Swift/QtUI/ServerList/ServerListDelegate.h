/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include <Swiften/Elements/StatusShow.h>

#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {

    class ServerListDelegate : public QStyledItemDelegate {
    public:
        ServerListDelegate();
        ~ServerListDelegate() override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    private:
        void paintServerAvatar(QPainter* painter, const QStyleOptionViewItem& option, const QString& avatarPath, const StatusShow& presence, bool isIdle, size_t unreadCount) const;
    private:
        DelegateCommons common_;
        static const int presenceIconHeight = 12;
        static const int presenceIconWidth = 12;
    };

}
