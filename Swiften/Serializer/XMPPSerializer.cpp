/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/XMPPSerializer.h>

#include <cassert>
#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Serializer/AuthChallengeSerializer.h>
#include <Swiften/Serializer/AuthFailureSerializer.h>
#include <Swiften/Serializer/AuthRequestSerializer.h>
#include <Swiften/Serializer/AuthResponseSerializer.h>
#include <Swiften/Serializer/AuthSuccessSerializer.h>
#include <Swiften/Serializer/ComponentHandshakeSerializer.h>
#include <Swiften/Serializer/CompressFailureSerializer.h>
#include <Swiften/Serializer/CompressRequestSerializer.h>
#include <Swiften/Serializer/EnableStreamManagementSerializer.h>
#include <Swiften/Serializer/IQSerializer.h>
#include <Swiften/Serializer/MessageSerializer.h>
#include <Swiften/Serializer/PresenceSerializer.h>
#include <Swiften/Serializer/StanzaAckRequestSerializer.h>
#include <Swiften/Serializer/StanzaAckSerializer.h>
#include <Swiften/Serializer/StartTLSFailureSerializer.h>
#include <Swiften/Serializer/StartTLSRequestSerializer.h>
#include <Swiften/Serializer/StreamErrorSerializer.h>
#include <Swiften/Serializer/StreamFeaturesSerializer.h>
#include <Swiften/Serializer/StreamManagementEnabledSerializer.h>
#include <Swiften/Serializer/StreamManagementFailedSerializer.h>
#include <Swiften/Serializer/StreamResumeSerializer.h>
#include <Swiften/Serializer/StreamResumedSerializer.h>
#include <Swiften/Serializer/TLSProceedSerializer.h>

namespace Swift {

XMPPSerializer::XMPPSerializer(PayloadSerializerCollection* payloadSerializers, StreamType type, bool setExplictNSonTopLevelElements) : type_(type) {
    serializers_.push_back(std::make_shared<PresenceSerializer>(payloadSerializers, setExplictNSonTopLevelElements ? getDefaultNamespace() : boost::optional<std::string>()));
    serializers_.push_back(std::make_shared<IQSerializer>(payloadSerializers, setExplictNSonTopLevelElements ? getDefaultNamespace() : boost::optional<std::string>()));
    serializers_.push_back(std::make_shared<MessageSerializer>(payloadSerializers, setExplictNSonTopLevelElements ? getDefaultNamespace() : boost::optional<std::string>()));
    serializers_.push_back(std::make_shared<CompressRequestSerializer>());
    serializers_.push_back(std::make_shared<CompressFailureSerializer>());
    serializers_.push_back(std::make_shared<AuthRequestSerializer>());
    serializers_.push_back(std::make_shared<AuthFailureSerializer>());
    serializers_.push_back(std::make_shared<AuthSuccessSerializer>());
    serializers_.push_back(std::make_shared<AuthChallengeSerializer>());
    serializers_.push_back(std::make_shared<AuthResponseSerializer>());
    serializers_.push_back(std::make_shared<StartTLSRequestSerializer>());
    serializers_.push_back(std::make_shared<StartTLSFailureSerializer>());
    serializers_.push_back(std::make_shared<TLSProceedSerializer>());
    serializers_.push_back(std::make_shared<StreamFeaturesSerializer>());
    serializers_.push_back(std::make_shared<StreamErrorSerializer>());
    serializers_.push_back(std::make_shared<EnableStreamManagementSerializer>());
    serializers_.push_back(std::make_shared<StreamManagementEnabledSerializer>());
    serializers_.push_back(std::make_shared<StreamManagementFailedSerializer>());
    serializers_.push_back(std::make_shared<StreamResumeSerializer>());
    serializers_.push_back(std::make_shared<StreamResumedSerializer>());
    serializers_.push_back(std::make_shared<StanzaAckSerializer>());
    serializers_.push_back(std::make_shared<StanzaAckRequestSerializer>());
    serializers_.push_back(std::make_shared<ComponentHandshakeSerializer>());
}

std::string XMPPSerializer::serializeHeader(const ProtocolHeader& header) const {
    std::string result = "<?xml version=\"1.0\"?><stream:stream xmlns=\"" + getDefaultNamespace() + "\" xmlns:stream=\"http://etherx.jabber.org/streams\"";
    if (!header.getFrom().empty()) {
        result += " from=\"" + header.getFrom() + "\"";
    }
    if (!header.getTo().empty()) {
        result += " to=\"" + header.getTo() + "\"";
    }
    if (!header.getID().empty()) {
        result += " id=\"" + header.getID() + "\"";
    }
    if (!header.getVersion().empty()) {
        result += " version=\"" + header.getVersion() + "\"";
    }
    result += ">";
    return result;
}

SafeByteArray XMPPSerializer::serializeElement(std::shared_ptr<ToplevelElement> element) const {
    std::vector< std::shared_ptr<ElementSerializer> >::const_iterator i = std::find_if(serializers_.begin(), serializers_.end(), boost::bind(&ElementSerializer::canSerialize, _1, element));
    if (i != serializers_.end()) {
        return (*i)->serialize(element);
    }
    else {
        SWIFT_LOG(warning) << "Could not find serializer for " << typeid(*(element.get())).name() << std::endl;
        return createSafeByteArray("");
    }
}

std::string XMPPSerializer::serializeFooter() const {
    return "</stream:stream>";
}

std::string XMPPSerializer::getDefaultNamespace() const {
    switch (type_) {
        case ClientStreamType: return "jabber:client";
        case ServerStreamType: return "jabber:server";
        case ComponentStreamType: return "jabber:component:accept";
    }
    assert(false);
    return "";
}

}
