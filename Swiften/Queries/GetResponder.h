/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Queries/Responder.h>

namespace Swift {
    template<typename T>
    class SWIFTEN_API GetResponder : public Responder<T> {
        public:
            GetResponder(IQRouter* router) : Responder<T>(router) {}

        private:
            virtual bool handleSetRequest(const JID&, const JID&, const std::string&, std::shared_ptr<T>) { return false; }
    };
}
