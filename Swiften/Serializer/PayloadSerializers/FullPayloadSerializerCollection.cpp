/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BlockSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BodySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BytestreamsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsDisableSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsEnableSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsPrivateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsReceivedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsSentSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ClientStateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CommandSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DelaySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptRequestSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DiscoItemsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/IBBSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/IdleSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/InBandRegistrationPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/IsodeIQDelegationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleContentPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferFileInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferHashSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleIBBTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleS5BTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/LastSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MAMFinSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MAMQuerySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MAMResultSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MIXParticipantSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCDestroyPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCInvitationPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCOwnerPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCUserPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/NicknameSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PrioritySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PrivateStorageSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPubSubSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RawXMLPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ReplaceSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RosterItemExchangeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/S5BProxyRequestSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StartSessionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StatusSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StatusShowSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StorageSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StreamInitiationFileInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StreamInitiationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SubjectSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ThreadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/UserLocationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/UserTuneSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/WhiteboardSerializer.h>

namespace Swift {

FullPayloadSerializerCollection::FullPayloadSerializerCollection() {
    serializers_.push_back(new IBBSerializer());
    serializers_.push_back(new BodySerializer());
    serializers_.push_back(new SubjectSerializer());
    serializers_.push_back(new ThreadSerializer());
    serializers_.push_back(new ChatStateSerializer());
    serializers_.push_back(new ClientStateSerializer());
    serializers_.push_back(new PrioritySerializer());
    serializers_.push_back(new ErrorSerializer(this));
    serializers_.push_back(new RosterSerializer());
    serializers_.push_back(new RosterItemExchangeSerializer());
    serializers_.push_back(new MUCPayloadSerializer());
    serializers_.push_back(new MUCDestroyPayloadSerializer());
    serializers_.push_back(new MUCAdminPayloadSerializer());
    serializers_.push_back(new MUCInvitationPayloadSerializer());
    serializers_.push_back(new MUCOwnerPayloadSerializer(this));
    serializers_.push_back(new MUCUserPayloadSerializer(this));
    serializers_.push_back(new SoftwareVersionSerializer());
    serializers_.push_back(new StatusSerializer());
    serializers_.push_back(new StatusShowSerializer());
    serializers_.push_back(new DiscoInfoSerializer());
    serializers_.push_back(new DiscoItemsSerializer());
    serializers_.push_back(new CapsInfoSerializer());
    serializers_.push_back(new BlockSerializer<BlockPayload>("block"));
    serializers_.push_back(new BlockSerializer<UnblockPayload>("unblock"));
    serializers_.push_back(new BlockSerializer<BlockListPayload>("blocklist"));
    serializers_.push_back(new ResourceBindSerializer());
    serializers_.push_back(new StartSessionSerializer());
    serializers_.push_back(new SecurityLabelSerializer());
    serializers_.push_back(new SecurityLabelsCatalogSerializer());
    serializers_.push_back(new StreamInitiationSerializer());
    serializers_.push_back(new BytestreamsSerializer());
    serializers_.push_back(new VCardSerializer());
    serializers_.push_back(new VCardUpdateSerializer());
    serializers_.push_back(new RawXMLPayloadSerializer());
    serializers_.push_back(new StorageSerializer());
    serializers_.push_back(new DelaySerializer());
    serializers_.push_back(new FormSerializer());
    serializers_.push_back(new PrivateStorageSerializer(this));
    serializers_.push_back(new CommandSerializer());
    serializers_.push_back(new InBandRegistrationPayloadSerializer());
    serializers_.push_back(new NicknameSerializer());
    serializers_.push_back(new SearchPayloadSerializer());
    serializers_.push_back(new ReplaceSerializer());
    serializers_.push_back(new LastSerializer());
    serializers_.push_back(new WhiteboardSerializer());
    serializers_.push_back(new UserLocationSerializer(this));
    serializers_.push_back(new UserTuneSerializer(this));
    serializers_.push_back(new IdleSerializer());

    serializers_.push_back(new StreamInitiationFileInfoSerializer());
    serializers_.push_back(new JingleFileTransferFileInfoSerializer());
    serializers_.push_back(new JingleContentPayloadSerializer());
    serializers_.push_back(new JingleFileTransferDescriptionSerializer());
    serializers_.push_back(new JingleFileTransferHashSerializer());
    serializers_.push_back(new JingleIBBTransportPayloadSerializer());
    serializers_.push_back(new JingleS5BTransportPayloadSerializer());
    serializers_.push_back(new JinglePayloadSerializer(this));
    serializers_.push_back(new S5BProxyRequestSerializer());
    serializers_.push_back(new DeliveryReceiptSerializer());
    serializers_.push_back(new DeliveryReceiptRequestSerializer());

    serializers_.push_back(new PubSubSerializer(this));
    serializers_.push_back(new PubSubEventSerializer(this));
    serializers_.push_back(new PubSubOwnerPubSubSerializer(this));
    serializers_.push_back(new PubSubErrorSerializer());

    serializers_.push_back(new ResultSetSerializer());
    serializers_.push_back(new ForwardedSerializer(this));
    serializers_.push_back(new MIXParticipantSerializer());
    serializers_.push_back(new MAMResultSerializer(this));
    serializers_.push_back(new MAMQuerySerializer());
    serializers_.push_back(new MAMFinSerializer());

    serializers_.push_back(new CarbonsDisableSerializer());
    serializers_.push_back(new CarbonsEnableSerializer());
    serializers_.push_back(new CarbonsPrivateSerializer());
    serializers_.push_back(new CarbonsReceivedSerializer(this));
    serializers_.push_back(new CarbonsSentSerializer(this));

    serializers_.push_back(new IsodeIQDelegationSerializer(this));

    for (auto serializer : serializers_) {
        addSerializer(serializer);
    }
}

FullPayloadSerializerCollection::~FullPayloadSerializerCollection() {
    for (auto serializer : serializers_) {
        removeSerializer(serializer);
        delete serializer;
    }
    serializers_.clear();
}

}
