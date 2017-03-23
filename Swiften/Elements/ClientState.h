/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API ClientState : public Payload {
        public:
            typedef std::shared_ptr<ClientState> ref;

        public:
            enum class ClientStateType {Active, Inactive};
            ClientState(ClientStateType state = ClientStateType::Active) : state_(state)  {

            }

            ClientStateType getClientState() const { return state_; }
            void setClientState(ClientStateType state) {state_ = state;}

        private:
            ClientStateType state_;
    };
}
