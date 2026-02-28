/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Component/CoreComponent.h>

namespace Swift {
    class SoftwareVersionResponder;

    /**
     * Provides the core functionality for writing XMPP component software.
     *
     * Besides connecting to an XMPP server, this class also provides interfaces for
     * performing most component tasks on the XMPP network.
     */
    class SWIFTEN_API Component : public CoreComponent {
        public:
            Component(const JID& jid, const std::string& secret, NetworkFactories* networkFactories);
            virtual ~Component();

            /**
             * Sets the software version of the client.
             *
             * This will be used to respond to version queries from other entities.
             */
            void setSoftwareVersion(const std::string& name, const std::string& version);

        private:
            SoftwareVersionResponder* softwareVersionResponder;
    };
}
