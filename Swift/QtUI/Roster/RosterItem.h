#pragma once

#include <QList>

namespace Swift {
	class RosterItem {
	public:
		RosterItem(RosterItem* parent);
		~RosterItem();
		void addChild(RosterItem* child);
		RosterItem* getParentItem();
		int rowCount();
		int rowOf(RosterItem* item);
		RosterItem* getItem(int row);
	private:
		QList<RosterItem*> children_;
		RosterItem* parent_;
	};
	
}