/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QAbstractListModel>
#include <QList>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

#include <Swift/QtUI/EventViewer/QtEvent.h>

namespace Swift {
class EventModel : public QAbstractListModel {
    Q_OBJECT
    public:
        EventModel();
        virtual ~EventModel();
        void addEvent(std::shared_ptr<StanzaEvent> event, bool active);
        void removeEvent(std::shared_ptr<StanzaEvent> event);
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        QtEvent* getItem(int row) const;
        int getNewEventCount();
    protected:
        QModelIndex index(int row, int column = 0, const QModelIndex & parent = QModelIndex()) const;
    private:
        QList<QtEvent*> activeEvents_;
        QList<QtEvent*> inactiveEvents_;
};

}
