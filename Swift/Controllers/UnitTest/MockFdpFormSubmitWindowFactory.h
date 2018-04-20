/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockFdpFormSubmitWindow.h>

namespace Swift {

    class MockFdpFormSubmitWindowFactory : public FdpFormSubmitWindowFactory {
        public:
            MockFdpFormSubmitWindowFactory() : FdpFormSubmitWindowFactory() {}

            virtual std::unique_ptr<FdpFormSubmitWindow> createFdpFormSubmitWindow() override {
                std::unique_ptr<FdpFormSubmitWindow> fdpFormSubmitWindow = std::make_unique<MockFdpFormSubmitWindow>();
                mockFdpFormSubmitWindow_ = static_cast<MockFdpFormSubmitWindow*>(fdpFormSubmitWindow.get());
                return fdpFormSubmitWindow;
            }

            MockFdpFormSubmitWindow* getMockFdpFormSubmitWindow() { return mockFdpFormSubmitWindow_; }

        private:
            MockFdpFormSubmitWindow* mockFdpFormSubmitWindow_;
    };
}
