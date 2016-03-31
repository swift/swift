/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    class NullNotifier : public Notifier {
        public:
            virtual void showMessage(Type, const std::string&, const std::string&, const boost::filesystem::path&, boost::function<void()>) {
            }
            virtual void purgeCallbacks() {
            }
    };
}
