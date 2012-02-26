/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtRemovableItemDelegate.h"

#include <QEvent>
#include <QPainter>

namespace Swift {

QtRemovableItemDelegate::QtRemovableItemDelegate(const QStyle* style) : style(style) {

}

void QtRemovableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex&) const {
	QStyleOption opt;
	opt.state = QStyle::State(0);
	opt.state |= QStyle::State_MouseOver;
	painter->save();
	painter->fillRect(option.rect, option.state & QStyle::State_Selected ? option.palette.highlight() : option.palette.base());
	painter->translate(option.rect.x(), option.rect.y()+(option.rect.height() - 12)/2);
	style->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, painter);
	painter->restore();
}

QWidget* QtRemovableItemDelegate::createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const {
	return NULL;
}

bool QtRemovableItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (event->type() == QEvent::MouseButtonRelease) {
		model->removeRow(index.row());
		return true;
	} else {
		return QItemDelegate::editorEvent(event, model, option, index);
	}
}

}
