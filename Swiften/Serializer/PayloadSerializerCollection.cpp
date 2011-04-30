/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <algorithm>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializer.h>

namespace Swift {

PayloadSerializerCollection::PayloadSerializerCollection() {
}

void PayloadSerializerCollection::addSerializer(PayloadSerializer* serializer) {
	serializers_.push_back(serializer);
}

void PayloadSerializerCollection::removeSerializer(PayloadSerializer* serializer) {
	serializers_.erase(remove(serializers_.begin(), serializers_.end(), serializer), serializers_.end());
}

PayloadSerializer* PayloadSerializerCollection::getPayloadSerializer(boost::shared_ptr<Payload> payload) const {
	std::vector<PayloadSerializer*>::const_iterator i = std::find_if(
			serializers_.begin(), serializers_.end(), 
			boost::bind(&PayloadSerializer::canSerialize, _1, payload));
	return (i != serializers_.end() ? *i : NULL);
}

}
