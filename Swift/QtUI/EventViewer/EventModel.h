/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QAbstractListModel>
#include <QList>

#include "Swiften/Events/StanzaEvent.h"

#include "Swift/QtUI/EventViewer/QtEvent.h"

namespace Swift {
class EventModel : public QAbstractListModel {
Q_OBJECT
public:
	EventModel();
	~EventModel();
	void addEvent(boost::shared_ptr<StanzaEvent> event, bool active);
	void removeEvent(boost::shared_ptr<StanzaEvent> event);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	QtEvent* getItem(int row) const;
	int getNewEventCount();
private:
	QList<QtEvent*> activeEvents_;
	QList<QtEvent*> inactiveEvents_;
};

}
