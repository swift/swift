/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {

/**
 * @brief The NotificationCenterNotifier class implmenents the notification interface for the
 *        OS X Notification Center API.
 */
class NotificationCenterNotifier : public Notifier {
public:
    NotificationCenterNotifier();
    virtual ~NotificationCenterNotifier();

    virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void ()> callback);
    virtual void purgeCallbacks();

    /**
     * @brief The handleUserNotificationActivated is called by the delegate, when a user activates/clicks on a notification.
     * @param identifier The std::string UUID identifiying the notification.
     */
    void handleUserNotificationActivated(const std::string& identifier);

private:
    class Private;
    const std::unique_ptr<Private> p;
};

}
