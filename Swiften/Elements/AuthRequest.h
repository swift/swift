/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API AuthRequest : public ToplevelElement {
        public:
            AuthRequest(const std::string& mechanism = "") : mechanism_(mechanism) {
            }

            AuthRequest(const std::string& mechanism, const SafeByteArray& message) :
                    mechanism_(mechanism), message_(message) {
            }

            AuthRequest(const std::string& mechanism, const boost::optional<SafeByteArray>& message) :
                    mechanism_(mechanism), message_(message) {
            }

            const boost::optional<SafeByteArray>& getMessage() const {
                return message_;
            }

            void setMessage(const SafeByteArray& message) {
                message_ = boost::optional<SafeByteArray>(message);
            }

            const std::string& getMechanism() const {
                return mechanism_;
            }

            void setMechanism(const std::string& mechanism) {
                mechanism_ = mechanism;
            }

        private:
            std::string mechanism_;
            boost::optional<SafeByteArray> message_;
    };
}
