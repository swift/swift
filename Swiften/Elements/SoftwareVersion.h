/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API SoftwareVersion : public Payload {
        public:
            typedef std::shared_ptr<SoftwareVersion> ref;

            SoftwareVersion(
                    const std::string& name = "",
                    const std::string& version = "",
                    const std::string& os = "") :
                        name_(name), version_(version), os_(os) {}

            const std::string& getName() const {
                return name_;
            }

            void setName(const std::string& name) {
                name_ = name;
            }

            const std::string& getVersion() const {
                return version_;
            }

            void setVersion(const std::string& version) {
                version_ = version;
            }

            const std::string& getOS() const {
                return os_;
            }

            void setOS(const std::string& os) {
                os_ = os;
            }

        private:
            std::string name_;
            std::string version_;
            std::string os_;
    };
}
