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
    class SWIFTEN_API SetResponder : public Responder<T> {
        public:
            SetResponder(IQRouter* router) : Responder<T>(router) {}

        private:
            virtual bool handleGetRequest(const JID&, const JID&, const std::string&, std::shared_ptr<T>) { return false; }
    };
}
