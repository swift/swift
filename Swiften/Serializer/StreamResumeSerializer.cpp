/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StreamResumeSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/StreamResume.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamResumeSerializer::StreamResumeSerializer() : GenericElementSerializer<StreamResume>() {
}

SafeByteArray StreamResumeSerializer::serialize(boost::shared_ptr<Element> el) const {
	boost::shared_ptr<StreamResume> e(boost::dynamic_pointer_cast<StreamResume>(el));
	XMLElement element("resume", "urn:xmpp:sm:2");
	element.setAttribute("previd", e->getResumeID());
	if (e->getHandledStanzasCount()) {
		element.setAttribute("h", boost::lexical_cast<std::string>(e->getHandledStanzasCount()));
	}
	return createSafeByteArray(element.serialize());
}
