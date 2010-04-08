/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/XMPPSerializer.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/CompressRequestSerializer.h"
#include "Swiften/Serializer/CompressFailureSerializer.h"
#include "Swiften/Serializer/StreamFeaturesSerializer.h"
#include "Swiften/Serializer/AuthRequestSerializer.h"
#include "Swiften/Serializer/AuthFailureSerializer.h"
#include "Swiften/Serializer/AuthSuccessSerializer.h"
#include "Swiften/Serializer/AuthChallengeSerializer.h"
#include "Swiften/Serializer/AuthResponseSerializer.h"
#include "Swiften/Serializer/StartTLSRequestSerializer.h"
#include "Swiften/Serializer/StartTLSFailureSerializer.h"
#include "Swiften/Serializer/TLSProceedSerializer.h"
#include "Swiften/Serializer/MessageSerializer.h"
#include "Swiften/Serializer/PresenceSerializer.h"
#include "Swiften/Serializer/IQSerializer.h"

namespace Swift {

XMPPSerializer::XMPPSerializer(PayloadSerializerCollection* payloadSerializers) {
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new PresenceSerializer(payloadSerializers)));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new IQSerializer(payloadSerializers)));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new MessageSerializer(payloadSerializers)));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new CompressRequestSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new CompressFailureSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new AuthRequestSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new AuthFailureSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new AuthSuccessSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new AuthChallengeSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new AuthResponseSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new StartTLSRequestSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new StartTLSFailureSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new TLSProceedSerializer()));
	serializers_.push_back(boost::shared_ptr<ElementSerializer>(new StreamFeaturesSerializer()));
}

String XMPPSerializer::serializeHeader(const ProtocolHeader& header) const {
	String result = "<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\"";
	if (!header.getFrom().isEmpty()) {
		result += " from=\"" + header.getFrom() + "\"";
	}
	if (!header.getTo().isEmpty()) {
		result += " to=\"" + header.getTo() + "\"";
	}
	if (!header.getID().isEmpty()) {
		result += " id=\"" + header.getID() + "\"";
	}
	if (!header.getVersion().isEmpty()) {
		result += " version=\"" + header.getVersion() + "\"";
	}
	result += ">";
	return result;
}

String XMPPSerializer::serializeElement(boost::shared_ptr<Element> element) const {
	std::vector< boost::shared_ptr<ElementSerializer> >::const_iterator i = std::find_if(
			serializers_.begin(), serializers_.end(), 
			boost::bind(&ElementSerializer::canSerialize, _1, element));
	if (i != serializers_.end()) {
		return (*i)->serialize(element);
	}
	else {
		std::cerr << "Could not find serializer for " << typeid(*(element.get())).name() << std::endl;
		return "";
	}
}

String XMPPSerializer::serializeFooter() const {
	return "</stream:stream>";
}

}
