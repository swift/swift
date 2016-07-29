/*
 * Copyright (c) 2015-2016 Isode Limited.
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

namespace boost {
    template<class T> class shared_ptr;
}

std::ostream& operator<<(std::ostream& os, const Swift::ClientError& error);

std::ostream& operator<<(std::ostream& os, Swift::Element* ele);

std::ostream& operator<<(std::ostream& os, Swift::ClientSession::State state);
