/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Form.h>

namespace Swift {
    class SWIFTEN_API MIXInformation : public PubSubPayload {
        public:

            MIXInformation();

            virtual ~MIXInformation();

            std::shared_ptr<Form> getData() const {
                return data;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data = value ;
            }


        private:
            std::shared_ptr<Form> data;
    };
}

