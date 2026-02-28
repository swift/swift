/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <iosfwd>

#include <Swiften/Client/ClientSession.h>

namespace Swift {
    class ClientError;
    class Element;
}

SWIFTEN_API std::ostream& operator<<(std::ostream& os, const Swift::ClientError& error);

SWIFTEN_API std::ostream& operator<<(std::ostream& os, Swift::Element* ele);

SWIFTEN_API std::ostream& operator<<(std::ostream& os, Swift::ClientSession::State state);
