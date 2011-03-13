/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class PayloadParserFactory;
	class PayloadSerializer;
	class FullPayloadParserFactoryCollection;
	class FullPayloadSerializerCollection;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;

	/** 
	 * The base class for XMPP entities (Clients, Components).
	 */
	class Entity  {
		public: 
			Entity();
			virtual ~Entity();

			void addPayloadParserFactory(PayloadParserFactory* payloadParserFactory);
			void removePayloadParserFactory(PayloadParserFactory* payloadParserFactory);

			void addPayloadSerializer(PayloadSerializer* payloadSerializer);
			void removePayloadSerializer(PayloadSerializer* payloadSerializer);

		protected:
			PayloadParserFactoryCollection* getPayloadParserFactories();
			PayloadSerializerCollection* getPayloadSerializers();

		private:
			FullPayloadParserFactoryCollection* payloadParserFactories;
			FullPayloadSerializerCollection* payloadSerializers;
	};
}
