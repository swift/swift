/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

namespace Swift {
    class FdpFormSubmitWindow;

    class FdpFormSubmitWindowFactory {
        public:
            virtual ~FdpFormSubmitWindowFactory() {}
            virtual std::unique_ptr<FdpFormSubmitWindow> createFdpFormSubmitWindow() = 0;
    };
}
