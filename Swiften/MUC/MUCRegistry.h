/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class JID;

    class SWIFTEN_API MUCRegistry {
        public:
            virtual ~MUCRegistry();

            bool isMUC(const JID& j) const;
            void addMUC(const JID& j);
            void removeMUC(const JID& j);

        private:
            std::vector<JID> mucs;
    };
}
