/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QPainter>
#include <QRect>
#include <QString>
#include <QStyleOptionViewItem>

namespace Swift {
    class DelegateCommons {
        public:
            DelegateCommons() : nameFont(QApplication::font()), detailFont(QApplication::font()), idleIcon(QIcon(":/icons/zzz.png")) {
                detailFontSizeDrop = nameFont.pointSize() >= 10 ? 2 : 0;
                detailFont.setStyle(QFont::StyleItalic);
                detailFont.setPointSize(nameFont.pointSize() - detailFontSizeDrop);
            }

            static void drawElidedText(QPainter* painter, const QRect& region, const QString& text, int flags = Qt::AlignTop);

            QSize contactSizeHint(const QStyleOptionViewItem& option, const QModelIndex& index, bool compact) const;
            void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QColor& nameColor, const QString& avatarPath, const QIcon& presenceIcon, const QString& name, const QString& statusText, bool isIdle, int unreadCount, bool compact) const;

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
            QIcon idleIcon;
    };
}
