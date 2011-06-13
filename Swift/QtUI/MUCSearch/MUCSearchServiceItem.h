/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QList>
#include <QString>

#include "Swift/QtUI/MUCSearch/MUCSearchRoomItem.h"

namespace Swift {
	class MUCSearchServiceItem : public MUCSearchItem {
		public:
			MUCSearchServiceItem(const QString& jidString) : jidString_(jidString) {}
			void addRoom(MUCSearchItem* room) {rooms_.push_back(room);}
			int rowCount() {return rooms_.count();}
			MUCSearchItem* getItem(int i) {return rooms_[i];}
			QVariant data(int role) {
				switch (role) {
					case Qt::DisplayRole: return QVariant(jidString_); 
					default: return QVariant();
				}
			}
			QString getHost() const {return jidString_;}
		private:
			QList<MUCSearchItem*> rooms_;
			QString jidString_;
	};
}
