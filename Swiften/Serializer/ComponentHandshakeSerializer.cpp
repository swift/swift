/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/ComponentHandshakeSerializer.h>

#include <Swiften/Elements/ComponentHandshake.h>

namespace Swift {

ComponentHandshakeSerializer::ComponentHandshakeSerializer() {
}

SafeByteArray ComponentHandshakeSerializer::serialize(std::shared_ptr<ToplevelElement> element)  const {
    std::shared_ptr<ComponentHandshake> handshake(std::dynamic_pointer_cast<ComponentHandshake>(element));
    return createSafeByteArray("<handshake>" + handshake->getData() + "</handshake>");
}

}
