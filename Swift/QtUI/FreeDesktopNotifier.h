/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Notifier/Notifier.h>

#include <Swift/QtUI/QtCachedImageScaler.h>

namespace Swift {
    class FreeDesktopNotifier : public Notifier {
        public:
            FreeDesktopNotifier(const std::string& name);

            virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
            virtual void purgeCallbacks() {}

        private:
            std::string applicationName;
            QtCachedImageScaler imageScaler;
    };
}
