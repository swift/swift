/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    /**
     * This class provides information about capabilities of entities on the network.
     * This information is provided in the form of service discovery
     * information.
     */
    class SWIFTEN_API EntityCapsProvider {
        public:
            virtual ~EntityCapsProvider();

            /**
             * Returns the service discovery information of the given JID.
             */
            virtual DiscoInfo::ref getCaps(const JID&) const = 0;

            virtual DiscoInfo::ref getCapsCached(const JID&) = 0;

            /**
             * Emitted when the capabilities of a JID changes.
             */
            boost::signals2::signal<void (const JID&)> onCapsChanged;
    };
}
