/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StreamResumedSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/StreamResumed.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamResumedSerializer::StreamResumedSerializer() : GenericElementSerializer<StreamResumed>() {
}

SafeByteArray StreamResumedSerializer::serialize(boost::shared_ptr<Element> el) const {
	boost::shared_ptr<StreamResumed> e(boost::dynamic_pointer_cast<StreamResumed>(el));
	XMLElement element("resumed", "urn:xmpp:sm:2");
	element.setAttribute("previd", e->getResumeID());
	if (e->getHandledStanzasCount()) {
		element.setAttribute("h", boost::lexical_cast<std::string>(e->getHandledStanzasCount()));
	}
	return createSafeByteArray(element.serialize());
}
