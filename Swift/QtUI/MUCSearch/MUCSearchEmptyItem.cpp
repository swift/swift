/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchEmptyItem.h>

#include <QColor>
#include <QFont>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

namespace Swift {
MUCSearchEmptyItem::MUCSearchEmptyItem(MUCSearchServiceItem* parent) : parent(parent) {
	parent->addRoom(this);
}

MUCSearchServiceItem* MUCSearchEmptyItem::getParent() {
	return parent;
}

QVariant MUCSearchEmptyItem::data(int role) {
	switch (role) {
		case Qt::DisplayRole:
			return QVariant(QObject::tr("No rooms found"));
		case Qt::FontRole: {
			QFont font;
			font.setItalic(true);
			return font;
		}
		case Qt::ForegroundRole:
			return QColor(Qt::gray);
		default:
			return QVariant();
	}
}

}
