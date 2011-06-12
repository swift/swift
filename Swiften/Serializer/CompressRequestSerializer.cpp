/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/CompressRequestSerializer.h>

#include <Swiften/Elements/CompressRequest.h>

namespace Swift {

CompressRequestSerializer::CompressRequestSerializer() {
}

SafeByteArray CompressRequestSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<CompressRequest> compressRequest(boost::dynamic_pointer_cast<CompressRequest>(element));
	return createSafeByteArray("<compress xmlns='http://jabber.org/protocol/compress'><method>" + compressRequest->getMethod() + "</method></compress>");
}

bool CompressRequestSerializer::canSerialize(boost::shared_ptr<Element> element) const {
	return boost::dynamic_pointer_cast<CompressRequest>(element) != 0;
}

}
