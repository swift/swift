/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/MUC/MUC.h>

namespace Swift {
    class IQRouter;
    class StanzaChannel;
    class DirectedPresenceSender;
    class MUCRegistry;

    class SWIFTEN_API MUCManager {
        public:
            MUCManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, MUCRegistry* mucRegistry);

            MUC::ref createMUC(const JID&);

        private:
            StanzaChannel* stanzaChannel;
            IQRouter* iqRouter;
            DirectedPresenceSender* presenceSender;
            MUCRegistry* mucRegistry;
    };
}
