/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StreamResumedSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamResumedSerializer::StreamResumedSerializer() : GenericElementSerializer<StreamResumed>() {
}

SafeByteArray StreamResumedSerializer::serialize(std::shared_ptr<ToplevelElement> el) const {
    std::shared_ptr<StreamResumed> e(std::dynamic_pointer_cast<StreamResumed>(el));
    XMLElement element("resumed", "urn:xmpp:sm:2");
    element.setAttribute("previd", e->getResumeID());
    if (e->getHandledStanzasCount()) {
        element.setAttribute("h", std::to_string(e->getHandledStanzasCount().get()));
    }
    return createSafeByteArray(element.serialize());
}
