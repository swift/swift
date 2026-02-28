/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

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
    class SWIFTEN_API Entity  {
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
