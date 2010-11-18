/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/MainWindow.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class RequestAdHocUIEvent : public UIEvent {
		public:
			RequestAdHocUIEvent(const DiscoItems::Item& command) : command_(command) {};
			const DiscoItems::Item& getCommand() const {return command_;}
		private:
			DiscoItems::Item command_;
	};
}
