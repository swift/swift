#pragma once

#include <QList>
#include <QVariant>
namespace Swift {
	class RosterItem : public QObject {
	Q_OBJECT
	public:
		RosterItem(RosterItem* parent);
		~RosterItem();
		void addChild(RosterItem* child);
		RosterItem* getParentItem();
		int rowCount();
		int rowOf(RosterItem* item);
		int row();
		RosterItem* getItem(int row);
		QVariant data(int role);
		void setName(QString name);
	signals:
		void changed();
	private:
		QList<RosterItem*> children_;
		RosterItem* parent_;
		QString name_;
	};
	
}