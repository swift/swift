/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/filesystem/fstream.hpp>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    /**
     * Preconditions for using growlnotifier:
     * - Must be part a bundle.
     * - The Carbon/Cocoa application loop must be running (e.g. through QApplication)
     *   such that notifications are coming through.
     */
    class GrowlNotifier : public Notifier {
        public:
            GrowlNotifier(const std::string& name);
            ~GrowlNotifier();

            virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
            virtual bool isExternallyConfigured() const;

            // Called by the delegate. Don't call.
            void handleNotificationClicked(void* data);
            void handleNotificationTimedOut(void* data);

            virtual void purgeCallbacks();

        private:
            void clearPendingNotifications();

        private:
            class Private;
            const std::unique_ptr<Private> p;
    };
}
