/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include <Swift/QtUI/EventViewer/TwoLineDelegate.h>
#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {
    enum EventType {MessageEventType, SubscriptionEventType, ErrorEventType, MUCInviteEventType, IncomingFileTransferEventType};
    class EventDelegate : public QStyledItemDelegate {
        Q_OBJECT
        public:
            EventDelegate();
            QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
            void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        private:
            EventType getEventType(std::shared_ptr<StanzaEvent> event) const;
            DelegateCommons common_;
            TwoLineDelegate messageDelegate_;
            TwoLineDelegate subscriptionDelegate_;
            TwoLineDelegate errorDelegate_;
            TwoLineDelegate mucInviteDelegate_;
            TwoLineDelegate incomingFileTransferDelegate_;
    };
}

