#pragma once

#include <QAbstractListModel>
#include <QList>

namespace Swift {
class EventModel : public QAbstractListModel {
Q_OBJECT
public:
	EventModel();
	~EventModel();
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
};

}
