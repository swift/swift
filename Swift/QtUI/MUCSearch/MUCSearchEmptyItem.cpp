/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchEmptyItem.h>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>
#include <QFont>
#include <QColor>

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
