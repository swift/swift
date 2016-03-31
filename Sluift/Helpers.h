/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>

namespace Swift {
    class ClientError;
    class ComponentError;

    std::string getErrorString(const ClientError& error);
    std::string getErrorString(const ComponentError& error);
}
