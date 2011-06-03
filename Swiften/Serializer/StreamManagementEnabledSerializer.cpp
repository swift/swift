/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StreamManagementEnabledSerializer.h>

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

StreamManagementEnabledSerializer::StreamManagementEnabledSerializer() : GenericElementSerializer<StreamManagementEnabled>() {
}

SafeByteArray StreamManagementEnabledSerializer::serialize(boost::shared_ptr<Element> el) const {
	boost::shared_ptr<StreamManagementEnabled> e(boost::dynamic_pointer_cast<StreamManagementEnabled>(el));
	XMLElement element("enabled", "urn:xmpp:sm:2");
	if (!e->getResumeID().empty()) {
		element.setAttribute("id", e->getResumeID());
	}
	if (e->getResumeSupported()) {
		element.setAttribute("resume", "true");
	}
	return createSafeByteArray(element.serialize());
}
