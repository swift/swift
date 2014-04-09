/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {

class Contact : public boost::enable_shared_from_this<Contact> {
	public:
		typedef boost::shared_ptr<Contact> ref;

		Contact();
		Contact(const std::string& name, const JID& jid, StatusShow::Type statusType, const boost::filesystem::path& path);

	public:
		std::string name;
		JID jid;
		StatusShow::Type statusType;
		boost::filesystem::path avatarPath;
};

}
