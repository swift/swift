/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MainWindow_H
#define SWIFTEN_MainWindow_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/StatusShow.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class Roster;

	class MainWindow {
		public:
			virtual ~MainWindow() {};
			virtual void setMyName(const String& name) = 0;
			virtual void setMyAvatarPath(const String& path) = 0;
			virtual void setMyStatusText(const String& status) = 0;
			virtual void setMyStatusType(StatusShow::Type type) = 0;
			virtual void setRosterModel(Roster* roster) = 0;
			
			boost::signal<void (const JID&)> onStartChatRequest;
			boost::signal<void (StatusShow::Type, const String&)> onChangeStatusRequest;
			boost::signal<void (bool)> onShowOfflineToggled;
			boost::signal<void ()> onSignOutRequest;
	};
}
#endif

