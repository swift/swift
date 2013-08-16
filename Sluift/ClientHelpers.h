/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>

#include <string>

namespace Swift {
	class ClientError;

	std::string getClientErrorString(const ClientError& error);
}
