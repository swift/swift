/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StreamErrorSerializer.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

StreamErrorSerializer::StreamErrorSerializer() : GenericElementSerializer<StreamError>() {
}

SafeByteArray StreamErrorSerializer::serialize(boost::shared_ptr<Element> element)  const {
	StreamError::ref error = boost::dynamic_pointer_cast<StreamError>(element);
	XMLElement errorElement("error", "http://etherx.jabber.org/streams");

	std::string typeTag;
	switch (error->getType()) {
		case StreamError::BadFormat: typeTag = "bad-format"; break;
		case StreamError::BadNamespacePrefix: typeTag = "bad-namespace-prefix"; break;
		case StreamError::Conflict: typeTag = "conflict"; break;
		case StreamError::ConnectionTimeout: typeTag = "connection-timeout"; break;
		case StreamError::HostGone: typeTag = "host-gone"; break;
		case StreamError::HostUnknown: typeTag = "host-unknown"; break;
		case StreamError::ImproperAddressing: typeTag = "improper-addressing"; break;
		case StreamError::InternalServerError: typeTag = "internal-server-error"; break;
		case StreamError::InvalidFrom: typeTag = "invalid-from"; break;
		case StreamError::InvalidID: typeTag = "invalid-id"; break;
		case StreamError::InvalidNamespace: typeTag = "invalid-namespace"; break;
		case StreamError::InvalidXML: typeTag = "invalid-xml"; break;
		case StreamError::NotAuthorized: typeTag = "not-authorized"; break;
		case StreamError::NotWellFormed: typeTag = "not-well-formed"; break;
		case StreamError::PolicyViolation: typeTag = "policy-violation"; break;
		case StreamError::RemoteConnectionFailed: typeTag = "remote-connection-failed"; break;
		case StreamError::Reset: typeTag = "reset"; break;
		case StreamError::ResourceConstraint: typeTag = "resource-constraint"; break;
		case StreamError::RestrictedXML: typeTag = "restricted-xml"; break;
		case StreamError::SeeOtherHost: typeTag = "see-other-host"; break;
		case StreamError::SystemShutdown: typeTag = "system-shutdown"; break;
		case StreamError::UndefinedCondition: typeTag = "undefined-condition"; break;
		case StreamError::UnsupportedEncoding: typeTag = "unsupported-encoding"; break;
		case StreamError::UnsupportedStanzaType: typeTag = "unsupported-stanza-type"; break;
		case StreamError::UnsupportedVersion: typeTag = "unsupported-version"; break;
	}
	errorElement.addNode(boost::make_shared<XMLElement>(typeTag, "urn:ietf:params:xml:ns:xmpp-streams"));	

	if (!error->getText().empty()) {
		errorElement.addNode(boost::make_shared<XMLElement>("text", "urn:ietf:params:xml:ns:xmpp-streams", error->getText()));
	}

	return createSafeByteArray(errorElement.serialize());
}

}
