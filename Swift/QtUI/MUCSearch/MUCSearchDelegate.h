/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

#include "Swift/QtUI/Roster/DelegateCommons.h"

namespace Swift {
	class MUCSearchDelegate : public QStyledItemDelegate {
		public:
			MUCSearchDelegate();
			~MUCSearchDelegate();
			/* QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const; */
			/* void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; */
		private:
//			void paintMUC(QPainter* painter, const QStyleOptionViewItem& option, MUCSearchMUCItem* item) const;
//			QSize mucSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const;

			DelegateCommons common_;
	};

}

