/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtRemovableItemDelegate.h"
#include <Swiften/Base/Platform.h>
#include <QEvent>
#include <QPainter>

namespace Swift {

QtRemovableItemDelegate::QtRemovableItemDelegate(const QStyle* style) : style(style) {

}

void QtRemovableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOption opt;
	opt.state = option.state;
	opt.state |= QStyle::State_AutoRaise;
	if (option.state.testFlag(QStyle::State_MouseOver)) {
		opt.state |= QStyle::State_Raised;
	}
	opt.rect = option.rect;
	painter->save();
	painter->fillRect(option.rect, option.state & QStyle::State_Selected ? option.palette.highlight() : option.palette.base());
	if (index.data().toString().isEmpty()) {
#ifdef SWIFTEN_PLATFORM_MACOSX
		// workaround for Qt not painting relative to the cell we're in, on OS X
		int height = style->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, 0);
		painter->translate(option.rect.x(), option.rect.y() + (option.rect.height() - height)/2);
#endif
		style->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, painter);
	}
	painter->restore();
}

QWidget* QtRemovableItemDelegate::createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const {
	return NULL;
}

bool QtRemovableItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (index.data().toString().isEmpty() && event->type() == QEvent::MouseButtonRelease) {
		model->removeRow(index.row());
		return true;
	} else {
		return QItemDelegate::editorEvent(event, model, option, index);
	}
}

QSize QtRemovableItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
	QSize size(style->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, 0) + 2, style->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, 0) + 2);
	return size;
}

}
