/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>

#include <string>

namespace Swift {
	class ClientError;
	class ComponentError;

	std::string getErrorString(const ClientError& error);
	std::string getErrorString(const ComponentError& error);
}
