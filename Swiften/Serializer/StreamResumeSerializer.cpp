/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StreamResumeSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/StreamResume.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamResumeSerializer::StreamResumeSerializer() : GenericElementSerializer<StreamResume>() {
}

SafeByteArray StreamResumeSerializer::serialize(std::shared_ptr<ToplevelElement> el) const {
    std::shared_ptr<StreamResume> e(std::dynamic_pointer_cast<StreamResume>(el));
    XMLElement element("resume", "urn:xmpp:sm:2");
    element.setAttribute("previd", e->getResumeID());
    if (e->getHandledStanzasCount()) {
        element.setAttribute("h", std::to_string(e->getHandledStanzasCount().get()));
    }
    return createSafeByteArray(element.serialize());
}
