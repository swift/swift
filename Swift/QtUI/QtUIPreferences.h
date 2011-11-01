/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>

namespace Swift {
	class QtUIPreferences : public QObject {
		Q_OBJECT
		public:
			QtUIPreferences();
			~QtUIPreferences();
			bool getCompactRosters();
		public slots:
			void setCompactRosters(bool compact);
		signals:
			void onCompactRostersChanged(bool /*now*/);
		private:
			bool compactRosters_;
	};
}