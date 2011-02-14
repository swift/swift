/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"

#include <vector>

#include <boost/optional.hpp>
#include <string>
#include "Swiften/JID/JID.h"
#include <Swift/Controllers/Chat/MUCSearchController.h>

namespace Swift {

	class MUCSearchWindow {
		public:
			virtual ~MUCSearchWindow() {};

			virtual void clearList() = 0;
			virtual void addService(const MUCService& service) = 0;
			virtual void addSavedServices(const std::list<JID>& services) = 0;
			virtual void setSearchInProgress(bool searching) = 0;

			virtual void show() = 0;

			boost::signal<void (const JID&)> onSearchService;
			boost::signal<void (const boost::optional<JID>&)> onFinished;
	};
}
