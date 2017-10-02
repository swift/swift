/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/function.hpp>

namespace Swift {
    class Notifier {
        public:
            virtual ~Notifier();

            enum Type { ContactAvailable, ContactUnavailable, ContactStatusChange, IncomingMessage, SystemMessage };

            /**
             * Picture is a PNG image.
             */
            virtual void showMessage(
                Type type,
                const std::string& subject,
                const std::string& description,
                const boost::filesystem::path& picture,
                boost::function<void()> callback) = 0;

            virtual bool isAvailable() const {
                return true;
            }

            virtual bool isExternallyConfigured() const {
                return false;
            }

            /** Remove any pending callbacks. */
            virtual void purgeCallbacks() = 0;

        protected:
            std::string typeToString(Type type);
            static std::vector<Type> getAllTypes();
            static std::vector<Type> getDefaultTypes();

            static const int DEFAULT_STATUS_NOTIFICATION_TIMEOUT_SECONDS;
            static const int DEFAULT_MESSAGE_NOTIFICATION_TIMEOUT_SECONDS;
    };
}
