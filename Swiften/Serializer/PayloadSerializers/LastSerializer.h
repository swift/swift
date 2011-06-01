/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Last.h>

namespace Swift {
	class LastSerializer : public GenericPayloadSerializer<Last> {
		public:
			LastSerializer() : GenericPayloadSerializer<Last>() {}

			virtual std::string serializePayload(boost::shared_ptr<Last> last)  const {
				return "<query xmlns='jabber:iq:last' seconds='" + boost::lexical_cast<std::string>(last->getSeconds()) + "'/>";
			}
	};
}
