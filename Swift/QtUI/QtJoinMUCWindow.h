/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>
#include <Swift/QtUI/ui_QtJoinMUCWindow.h>

namespace Swift {
	class QtJoinMUCWindow : public QWidget, public JoinMUCWindow {
			Q_OBJECT
		public:
			QtJoinMUCWindow();

			virtual void setNick(const std::string& nick);
			virtual void setMUC(const std::string& nick);

			virtual void show();

		private slots:
			void handleJoin();
			void handleSearch();

		private:
			Ui::QtJoinMUCWindow ui;
			std::string lastSetNick;
	};
}
