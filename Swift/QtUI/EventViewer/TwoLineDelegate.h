/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include "Swift/QtUI/Roster/DelegateCommons.h"
#include "QtEvent.h"

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

