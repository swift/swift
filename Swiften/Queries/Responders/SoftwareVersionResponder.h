/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/GetResponder.h>

namespace Swift {
    class IQRouter;

    class SWIFTEN_API SoftwareVersionResponder : public GetResponder<SoftwareVersion> {
        public:
            SoftwareVersionResponder(IQRouter* router);

            void setVersion(const std::string& client, const std::string& version, const std::string& os = "");

        private:
            virtual bool handleGetRequest(const JID& from, const JID& to, const std::string& id, std::shared_ptr<SoftwareVersion> payload);

        private:
            std::string client;
            std::string version;
            std::string os;
    };
}
