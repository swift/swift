/*
 * Copyright (c) 2013-2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>

#include <string>

namespace Swift {
	class ClientError;
	class ComponentError;

	template<typename T> std::string getCommonErrorString(T& error);
	std::string getErrorString(const ClientError& error);
	std::string getErrorString(const ComponentError& error);
}
