/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/Event.h>

#include <iostream>
#include <typeinfo>

std::ostream& operator<<(std::ostream& os, const Swift::Event& e) {
    os << "Event(" << e.id << ",";
    if (e.owner) {
        os << typeid(*e.owner.get()).name();
    }
    else {
        os << "null";
    }
    os << ")";
    return os;
}
