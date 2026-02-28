/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Version : public Payload {
        public:
            Version(const std::string& name = "", const std::string& version = "", const std::string& os = "") : name_(name), version_(version), os_(os) { }

            const std::string& getName() const { return name_; }
            const std::string& getVersion() const { return version_; }
            const std::string& getOS() const { return os_; }

        private:
            std::string name_;
            std::string version_;
            std::string os_;
    };
}
