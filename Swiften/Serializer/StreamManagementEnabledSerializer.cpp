/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StreamManagementEnabledSerializer.h>

#include <memory>

#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamManagementEnabledSerializer::StreamManagementEnabledSerializer() : GenericElementSerializer<StreamManagementEnabled>() {
}

SafeByteArray StreamManagementEnabledSerializer::serialize(std::shared_ptr<ToplevelElement> el) const {
    std::shared_ptr<StreamManagementEnabled> e(std::dynamic_pointer_cast<StreamManagementEnabled>(el));
    XMLElement element("enabled", "urn:xmpp:sm:2");
    if (!e->getResumeID().empty()) {
        element.setAttribute("id", e->getResumeID());
    }
    if (e->getResumeSupported()) {
        element.setAttribute("resume", "true");
    }
    return createSafeByteArray(element.serialize());
}
