/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Queries/IQChannel.h>

namespace Swift {
    class DummyIQChannel : public IQChannel {
        public:
            DummyIQChannel() {}

            virtual void sendIQ(std::shared_ptr<IQ> iq) {
                iqs_.push_back(iq);
            }

            virtual std::string getNewIQID() {
                return "test-id";
            }

            virtual bool isAvailable() const {
                return true;
            }

            std::vector<std::shared_ptr<IQ> > iqs_;
    };
}
