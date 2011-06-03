/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class StanzaAckSerializer : public GenericElementSerializer<StanzaAck> {
		public:
			StanzaAckSerializer() : GenericElementSerializer<StanzaAck>() {
			}

			virtual SafeByteArray serialize(boost::shared_ptr<Element> element) const {
				StanzaAck::ref stanzaAck(boost::dynamic_pointer_cast<StanzaAck>(element));
				assert(stanzaAck->isValid());
				XMLElement result("a", "urn:xmpp:sm:2");
				result.setAttribute("h", std::string(boost::lexical_cast<std::string>(stanzaAck->getHandledStanzasCount())));
				return createSafeByteArray(result.serialize());
			}
	};
}
