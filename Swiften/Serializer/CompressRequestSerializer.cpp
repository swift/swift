/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/CompressRequestSerializer.h>

#include <Swiften/Elements/CompressRequest.h>

namespace Swift {

CompressRequestSerializer::CompressRequestSerializer() {
}

SafeByteArray CompressRequestSerializer::serialize(boost::shared_ptr<ToplevelElement> element)  const {
    boost::shared_ptr<CompressRequest> compressRequest(boost::dynamic_pointer_cast<CompressRequest>(element));
    return createSafeByteArray("<compress xmlns='http://jabber.org/protocol/compress'><method>" + compressRequest->getMethod() + "</method></compress>");
}

bool CompressRequestSerializer::canSerialize(boost::shared_ptr<ToplevelElement> element) const {
    return boost::dynamic_pointer_cast<CompressRequest>(element) != 0;
}

}
