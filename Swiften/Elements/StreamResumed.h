/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API StreamResumed : public ToplevelElement {
        public:
            StreamResumed();
            ~StreamResumed();

            void setResumeID(const std::string& id) {
                resumeID = id;
            }

            const std::string& getResumeID() const {
                return resumeID;
            }

            const boost::optional<unsigned int> getHandledStanzasCount() const {
                return handledStanzasCount;
            }

            void setHandledStanzasCount(unsigned int i) {
                handledStanzasCount = i;
            }

        private:
            std::string resumeID;
            boost::optional<unsigned int> handledStanzasCount;
    };
}
