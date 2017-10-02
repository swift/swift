/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXUserPreference : public Payload {
        public:
            using ref = std::shared_ptr<MIXUserPreference>;

        public:

            MIXUserPreference() {}

            std::shared_ptr<Form> getData() const {
                return data_;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data_ = value ;
            }


        private:
            std::shared_ptr<Form> data_;
    };
}
