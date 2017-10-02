/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include <Swiften/Network/Connection.h>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
    class ConnectionFactory;

    class GNTPNotifier : public Notifier {
        public:
            GNTPNotifier(const std::string& name, const boost::filesystem::path& icon, ConnectionFactory* connectionFactory);
            ~GNTPNotifier();

            virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);

        private:
            void handleConnectFinished(bool error);
            void handleDataRead(const ByteArray& data);
            void send(const std::string& message);

        private:
            std::string name;
            boost::filesystem::path icon;
            ConnectionFactory* connectionFactory;
            bool initialized;
            bool registered;
            std::string currentMessage;
            Connection::ref currentConnection;
    };
}
