/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>
#include <QColor>
#include <QFont>

#include "DelegateCommons.h"

namespace Swift {
	class QtTreeWidgetItem;
	class GroupItemDelegate {
	public:
		GroupItemDelegate();
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QString& name, int rowCount, bool expanded) const;
	private:
		void paintShadowText(QPainter* painter, const QRect& region, const QString& text) const;
		void paintExpansionTriangle(QPainter* painter, const QRect& region, int width, int height, bool expanded) const;
		QFont groupFont_;
		static const int groupCornerRadius_;
		DelegateCommons common_;
	};
}
