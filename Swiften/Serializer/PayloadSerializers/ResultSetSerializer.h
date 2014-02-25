/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/ResultSet.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API ResultSetSerializer : public GenericPayloadSerializer<ResultSet> {
		public:
			ResultSetSerializer();
			virtual ~ResultSetSerializer();

			virtual std::string serializePayload(boost::shared_ptr<ResultSet>) const SWIFTEN_OVERRIDE;
	};
}
