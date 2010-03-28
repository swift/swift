#include "Swiften/Serializer/CompressRequestSerializer.h"

#include "Swiften/Elements/CompressRequest.h"

namespace Swift {

CompressRequestSerializer::CompressRequestSerializer() {
}

String CompressRequestSerializer::serialize(boost::shared_ptr<Element> element)  const {
	boost::shared_ptr<CompressRequest> compressRequest(boost::dynamic_pointer_cast<CompressRequest>(element));
	return "<compress xmlns='http://jabber.org/protocol/compress'><method>" + compressRequest->getMethod() + "</method></compress>";
}

bool CompressRequestSerializer::canSerialize(boost::shared_ptr<Element> element) const {
	return dynamic_cast<CompressRequest*>(element.get()) != 0;
}

}
