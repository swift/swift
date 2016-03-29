/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/StreamResumeSerializer.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamResume.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamResumeSerializer::StreamResumeSerializer() : GenericElementSerializer<StreamResume>() {
}

SafeByteArray StreamResumeSerializer::serialize(boost::shared_ptr<ToplevelElement> el) const {
	boost::shared_ptr<StreamResume> e(boost::dynamic_pointer_cast<StreamResume>(el));
	XMLElement element("resume", "urn:xmpp:sm:2");
	element.setAttribute("previd", e->getResumeID());
	if (e->getHandledStanzasCount()) {
		element.setAttribute("h", boost::lexical_cast<std::string>(e->getHandledStanzasCount().get()));
	}
	return createSafeByteArray(element.serialize());
}
