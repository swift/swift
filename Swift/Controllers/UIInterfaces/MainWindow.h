/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/StatusShow.h"
#include "Swiften/Elements/DiscoItems.h"

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {
	class Roster;

	class MainWindow {
		public:
			MainWindow(bool candelete = true) : canDelete_(candelete) {}
			virtual ~MainWindow() {};

			bool canDelete() const {
				return canDelete_;
			}

			virtual void setMyNick(const std::string& name) = 0;
			virtual void setMyJID(const JID& jid) = 0;
			virtual void setMyAvatarPath(const std::string& path) = 0;
			virtual void setMyStatusText(const std::string& status) = 0;
			virtual void setMyStatusType(StatusShow::Type type) = 0;
			/** Must be able to cope with NULL to clear the roster */
			virtual void setRosterModel(Roster* roster) = 0;
			virtual void setConnecting() = 0;
			virtual void setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands) = 0;
			
			boost::signal<void (StatusShow::Type, const std::string&)> onChangeStatusRequest;
			boost::signal<void ()> onSignOutRequest;

		private:
			bool canDelete_;
	};
}
