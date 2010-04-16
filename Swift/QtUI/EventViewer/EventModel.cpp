/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "EventModel.h"

#include <QtDebug>

namespace Swift {
EventModel::EventModel() {
	
}

EventModel::~EventModel() {
	foreach (QtEvent* event, activeEvents_) {
		delete event;
	}
	foreach (QtEvent* event, inactiveEvents_) {
		delete event;
	}
}

QtEvent* EventModel::getItem(int row) const {
	return row < activeEvents_.size() ? activeEvents_[row] : inactiveEvents_[row - activeEvents_.size()];
}

int EventModel::getNewEventCount() {
	return activeEvents_.size();
}

QVariant EventModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid()) {
		return QVariant();
	}
	QtEvent* item = getItem(index.row());
	QVariant result = item ? item->data(role) : QVariant();
	return result;
}

/*
 * We only reimplement this to get the pointers inside the indices.
 */
QModelIndex EventModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent) || parent.isValid()) {
		return QModelIndex();
	}

	return row < rowCount() ? createIndex(row, column, getItem(row)) : QModelIndex();
}

int EventModel::rowCount(const QModelIndex& parent) const {
	/* Invalid parent = root, valid parent = child, and we're a list not a tree.*/
	int count = parent.isValid() ? 0 : activeEvents_.size() + inactiveEvents_.size();
	return count;
}

void EventModel::addEvent(boost::shared_ptr<StanzaEvent> event, bool active) {
	if (active) {
		activeEvents_.push_front(new QtEvent(event, active));
		emit dataChanged(createIndex(0, 0), createIndex(1, 0));
	} else {
		inactiveEvents_.push_front(new QtEvent(event, active));
		emit dataChanged(createIndex(activeEvents_.size() -1, 0), createIndex(activeEvents_.size(), 0));
		if (inactiveEvents_.size() > 50) {
			removeEvent(inactiveEvents_[20]->getEvent());
		}
	}
	emit layoutChanged();
}

void EventModel::removeEvent(boost::shared_ptr<StanzaEvent> event) {
	for (int i = inactiveEvents_.size() - 1; i >= 0; i--) {
		if (event == inactiveEvents_[i]->getEvent()) {
			inactiveEvents_.removeAt(i);
			emit dataChanged(createIndex(activeEvents_.size() + i - 1, 0), createIndex(activeEvents_.size() + i - 1, 0));
			return;
		}
	}

	for (int i = 0; i < activeEvents_.size(); i++) {
		if (event == activeEvents_[i]->getEvent()) {
			activeEvents_.removeAt(i);
			emit dataChanged(createIndex(i - 1, 0), createIndex(i - 1, 0));
			return;
		}
	}

}

}
