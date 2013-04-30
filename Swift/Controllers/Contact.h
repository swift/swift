/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {

class Contact {
	public:
		Contact();
		Contact(const std::string& name, const JID& jid, StatusShow::Type statusType, const boost::filesystem::path& path);

	public:
		std::string name;
		JID jid;
		StatusShow::Type statusType;
		boost::filesystem::path avatarPath;
};

}
