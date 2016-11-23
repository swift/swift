/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/BytestreamsSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

BytestreamsSerializer::BytestreamsSerializer() {
}

std::string BytestreamsSerializer::serializePayload(std::shared_ptr<Bytestreams> bytestreams)    const {
    XMLElement queryElement("query", "http://jabber.org/protocol/bytestreams");
    queryElement.setAttribute("sid", bytestreams->getStreamID());
    for (const auto& streamHost : bytestreams->getStreamHosts()) {
        std::shared_ptr<XMLElement> streamHostElement(new XMLElement("streamhost"));
        streamHostElement->setAttribute("host", streamHost.host);
        streamHostElement->setAttribute("jid", streamHost.jid.toString());
        streamHostElement->setAttribute("port", boost::lexical_cast<std::string>(streamHost.port));
        queryElement.addNode(streamHostElement);
    }

    if (bytestreams->getUsedStreamHost()) {
        std::shared_ptr<XMLElement> streamHostElement(new XMLElement("streamhost-used"));
        streamHostElement->setAttribute("jid", *bytestreams->getUsedStreamHost());
        queryElement.addNode(streamHostElement);
    }
    return queryElement.serialize();
}

}
