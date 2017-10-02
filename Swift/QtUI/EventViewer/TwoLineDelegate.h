/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include <Swift/QtUI/EventViewer/QtEvent.h>
#include <Swift/QtUI/Roster/DelegateCommons.h>

namespace Swift {
    class TwoLineDelegate {
        public:
            TwoLineDelegate(int firstRole, int secondRole, bool wrap);
            ~TwoLineDelegate();
            QSize sizeHint(const QStyleOptionViewItem& option, QtEvent* event) const;
            void paint(QPainter* painter, const QStyleOptionViewItem& option, QtEvent* event) const;
        private:
            DelegateCommons common_;
            int firstRole_;
            int secondRole_;
            bool wrap_;
    };

}

