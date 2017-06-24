/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXCreate : public Payload {
        public:
            using ref = std::shared_ptr<MIXCreate>;

        public:

            MIXCreate() {}

            std::shared_ptr<Form> getData() const {
                return data_;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data_ = value ;
            }

            const boost::optional<std::string>& getChannel() const {
                return channel_;
            }

            void setChannel(const std::string& channel) {
                this->channel_ = channel;
            }

        private:
            boost::optional<std::string> channel_;
            std::shared_ptr<Form> data_;
    };
}
