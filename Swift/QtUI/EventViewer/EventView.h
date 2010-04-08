/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QListView>

namespace Swift {
	class EventView : public QListView {
		Q_OBJECT
		public:
			EventView(QWidget* parent);
	};
}
