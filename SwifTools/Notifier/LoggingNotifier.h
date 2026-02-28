/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    class LoggingNotifier : public Notifier {
        public:
            virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback) {
                notifications.push_back(Notification(type, subject, description, picture, callback));
            }

            struct Notification {
                    Notification(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback) : type(type), subject(subject), description(description), picture(picture), callback(callback) {}
                    Type type;
                    std::string subject;
                    std::string description;
                    boost::filesystem::path picture;
                    boost::function<void()> callback;
            };

            virtual void purgeCallbacks() {}

            std::vector<Notification> notifications;
    };
}
