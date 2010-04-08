/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/PayloadSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/BodySerializer.h"
#include "Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/PrioritySerializer.h"
#include "Swiften/Serializer/PayloadSerializers/ErrorSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/RosterSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/StatusSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/StatusShowSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/StartSessionSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/RawXMLPayloadSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/StorageSerializer.h"
#include "Swiften/Serializer/PayloadSerializers/PrivateStorageSerializer.h"

namespace Swift {

FullPayloadSerializerCollection::FullPayloadSerializerCollection() {
	serializers_.push_back(new BodySerializer());
	serializers_.push_back(new ChatStateSerializer());
	serializers_.push_back(new PrioritySerializer());
	serializers_.push_back(new ErrorSerializer());
	serializers_.push_back(new RosterSerializer());
	serializers_.push_back(new MUCPayloadSerializer());
	serializers_.push_back(new SoftwareVersionSerializer());
	serializers_.push_back(new StatusSerializer());
	serializers_.push_back(new StatusShowSerializer());
	serializers_.push_back(new DiscoInfoSerializer());
	serializers_.push_back(new CapsInfoSerializer());
	serializers_.push_back(new ResourceBindSerializer());
	serializers_.push_back(new StartSessionSerializer());
	serializers_.push_back(new SecurityLabelSerializer());
	serializers_.push_back(new SecurityLabelsCatalogSerializer());
	serializers_.push_back(new VCardSerializer());
	serializers_.push_back(new VCardUpdateSerializer());
	serializers_.push_back(new RawXMLPayloadSerializer());
	serializers_.push_back(new StorageSerializer());
	serializers_.push_back(new PrivateStorageSerializer(this));
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
