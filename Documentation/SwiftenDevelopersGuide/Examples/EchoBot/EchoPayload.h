/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
//...
#pragma once

#include <Swiften/Swiften.h>

//...
class EchoPayload : public Swift::Payload {
    public:
        EchoPayload() {}

        const std::string& getMessage() const {
            return message;
        }

        void setMessage(const std::string& message) {
            this->message = message;
        }

    private:
        std::string message;
};
