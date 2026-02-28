/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/CompressRequestSerializer.h>

#include <Swiften/Elements/CompressRequest.h>

namespace Swift {

CompressRequestSerializer::CompressRequestSerializer() {
}

SafeByteArray CompressRequestSerializer::serialize(std::shared_ptr<ToplevelElement> element)  const {
    std::shared_ptr<CompressRequest> compressRequest(std::dynamic_pointer_cast<CompressRequest>(element));
    return createSafeByteArray("<compress xmlns='http://jabber.org/protocol/compress'><method>" + compressRequest->getMethod() + "</method></compress>");
}

bool CompressRequestSerializer::canSerialize(std::shared_ptr<ToplevelElement> element) const {
    return std::dynamic_pointer_cast<CompressRequest>(element) != nullptr;
}

}
