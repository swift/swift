/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/EventViewer/EventModel.h>

#include <Swiften/Base/Log.h>

namespace Swift {

namespace {
    const int inactiveEventsLimit = 50;
}

EventModel::EventModel() {

}

EventModel::~EventModel() {
    for (auto event : activeEvents_) {
        delete event;
    }
    for (auto event : inactiveEvents_) {
        delete event;
    }
}

QtEvent* EventModel::getItem(int row) const {
    QtEvent* event = nullptr;
    if (row < activeEvents_.size()) {
        event = activeEvents_[row];
    }
    else {
        int inactiveRow = row - activeEvents_.size();
        if (inactiveRow < inactiveEvents_.size()) {
            event = inactiveEvents_[inactiveRow];
        }
        else {
            SWIFT_LOG(error) << "Misbehaving EventModel requests row index outside of range";
        }
    }
    return event;
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

void EventModel::addEvent(std::shared_ptr<StanzaEvent> event, bool active) {
    beginResetModel();
    if (active) {
        activeEvents_.push_front(new QtEvent(event, active));
    } else {
        inactiveEvents_.push_front(new QtEvent(event, active));
        if (inactiveEvents_.size() > inactiveEventsLimit) {
            removeEvent(inactiveEvents_[inactiveEventsLimit]->getEvent());
        }
    }
    endResetModel();
}

void EventModel::removeEvent(std::shared_ptr<StanzaEvent> event) {
    beginResetModel();
    for (int i = inactiveEvents_.size() - 1; i >= 0; i--) {
        if (event == inactiveEvents_[i]->getEvent()) {
            inactiveEvents_.removeAt(i);
            endResetModel();
            return;
        }
    }

    for (int i = 0; i < activeEvents_.size(); i++) {
        if (event == activeEvents_[i]->getEvent()) {
            activeEvents_.removeAt(i);
            endResetModel();
            return;
        }
    }
    endResetModel();
}

}
