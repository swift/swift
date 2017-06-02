/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXInformation : public Payload {
        public:
            typedef std::shared_ptr<MIXInformation> ref;

            enum JIDVisibility {
                Hidden,
                Maybe,
                Visible
            };

            struct Item {
                std::string name;
                std::string description;
                JIDVisibility jidVisibility;
                std::vector<JID> contacts;
            };

            MIXInformation() {}

        private:
            std::shared_ptr<Item> item;

    };
}