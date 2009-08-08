#pragma once

#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"

#include <QAbstractItemModel>
#include <QList>

namespace Swift {
class RosterModel : public QAbstractItemModel {
Q_OBJECT
public:
	RosterModel();
	~RosterModel();
	void setRoot(QtTreeWidgetItem* tree);
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex index(QtTreeWidgetItem* item) const;
	QModelIndex parent(const QModelIndex& index) const;
	QModelIndex parent(QtTreeWidgetItem* item) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
signals:
	void itemExpanded(const QModelIndex& item, bool expanded);
private slots:
	void handleItemChanged(QtTreeWidgetItem* item);
private:
	QtTreeWidgetItem* tree_;
};

}