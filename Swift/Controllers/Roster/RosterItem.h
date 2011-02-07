/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
class GroupRosterItem;
class RosterItem {
	public:
		RosterItem(const String& name, GroupRosterItem* parent);
		virtual ~RosterItem();
		boost::signal<void ()> onDataChanged;
		GroupRosterItem* getParent() const;
		void setDisplayName(const String& name);
		String getDisplayName() const;
		String getSortableDisplayName() const;
	private:
		String name_;
		String sortableDisplayName_;
		GroupRosterItem* parent_;
};

}

