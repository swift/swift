/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/IBBSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BodySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BlockSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SubjectSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PrioritySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RosterItemExchangeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCUserPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCOwnerPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCDestroyPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MUCInvitationPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StatusSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StatusShowSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DiscoItemsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StartSessionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StreamInitiationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/BytestreamsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/RawXMLPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/StorageSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PrivateStorageSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DelaySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CommandSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/InBandRegistrationPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/NicknameSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ReplaceSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/LastSerializer.h>

#include <Swiften/Serializer/PayloadSerializers/StreamInitiationFileInfoSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleContentPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferDescriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferHashSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferReceivedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleIBBTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleS5BTransportPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/S5BProxyRequestSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptRequestSerializer.h>

namespace Swift {

FullPayloadSerializerCollection::FullPayloadSerializerCollection() {
	serializers_.push_back(new IBBSerializer());
	serializers_.push_back(new BodySerializer());
	serializers_.push_back(new SubjectSerializer());
	serializers_.push_back(new ChatStateSerializer());
	serializers_.push_back(new PrioritySerializer());
	serializers_.push_back(new ErrorSerializer());
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
	
	serializers_.push_back(new StreamInitiationFileInfoSerializer());
	serializers_.push_back(new JingleContentPayloadSerializer());
	serializers_.push_back(new JingleFileTransferDescriptionSerializer());
	serializers_.push_back(new JingleFileTransferHashSerializer());
	serializers_.push_back(new JingleFileTransferReceivedSerializer());
	serializers_.push_back(new JingleIBBTransportPayloadSerializer());
	serializers_.push_back(new JingleS5BTransportPayloadSerializer());
	serializers_.push_back(new JinglePayloadSerializer(this));
	serializers_.push_back(new S5BProxyRequestSerializer());
	serializers_.push_back(new DeliveryReceiptSerializer());
	serializers_.push_back(new DeliveryReceiptRequestSerializer());
	
	foreach(PayloadSerializer* serializer, serializers_) {
		addSerializer(serializer);
	}
}

FullPayloadSerializerCollection::~FullPayloadSerializerCollection() {
	foreach(PayloadSerializer* serializer, serializers_) {
		removeSerializer(serializer);
		delete serializer;
	}
	serializers_.clear();
}

}
