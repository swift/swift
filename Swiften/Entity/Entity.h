/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

namespace Swift {
	class PayloadParserFactory;
	class PayloadSerializer;

	/** 
	 * The base class for XMPP entities (Clients, Components).
	 */
	class Entity  {
		public: 
			virtual ~Entity();

			void addPayloadParserFactory(PayloadParserFactory* payloadParserFactory);
			void removePayloadParserFactory(PayloadParserFactory* payloadParserFactory);

			void addPayloadSerializer(PayloadSerializer* payloadSerializer);
			void removePayloadSerializer(PayloadSerializer* payloadSerializer);

		protected:
			PayloadParserFactoryCollection* getPayloadParserFactories() {
				return &payloadParserFactories;
			}

			PayloadSerializerCollection* getPayloadSerializers() {
				return &payloadSerializers;
			}

		private:
			FullPayloadParserFactoryCollection payloadParserFactories;
			FullPayloadSerializerCollection payloadSerializers;
	};
}
