/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QApplication>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QString>
#include <QIcon>
#include <QStyleOptionViewItem>

namespace Swift {
	class DelegateCommons {
		public:
			DelegateCommons() : nameFont(QApplication::font()), detailFont(QApplication::font()) {
				detailFontSizeDrop = nameFont.pointSize() >= 10 ? 2 : 0;
				detailFont.setStyle(QFont::StyleItalic);
				detailFont.setPointSize(nameFont.pointSize() - detailFontSizeDrop);
			}

			static void drawElidedText(QPainter* painter, const QRect& region, const QString& text, int flags = Qt::AlignTop);

			QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index, bool compact) const;
			void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QColor& nameColor, const QString& avatarPath, const QIcon& presenceIcon, const QString& name, const QString& statusText, int unreadCount, bool compact) const;

			int detailFontSizeDrop;
			QFont nameFont;
			QFont detailFont;
			static const int horizontalMargin;
			static const int verticalMargin;
			static const int farLeftMargin;			
			static const int avatarSize;
			static const int presenceIconHeight;
			static const int presenceIconWidth;
			static const int unreadCountSize;
	};
}
