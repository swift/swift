/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include "Swift/QtUI/Roster/DelegateCommons.h"
#include "Swift/QtUI/EventViewer/TwoLineDelegate.h"

namespace Swift {
	enum EventType {MessageEventType, SubscriptionEventType, ErrorEventType};
	class EventDelegate : public QStyledItemDelegate {
		Q_OBJECT
		public:
			EventDelegate();
			QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		private:
			EventType getEventType(boost::shared_ptr<StanzaEvent> event) const;
			DelegateCommons common_;
			TwoLineDelegate messageDelegate_;
			TwoLineDelegate subscriptionDelegate_;
			TwoLineDelegate errorDelegate_;
	};
}

