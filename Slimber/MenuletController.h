/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/signals2.hpp>

class Menulet;

class MenuletController {
    public:
        enum Status {
            Online,
            Offline
        };

        MenuletController(Menulet*);
        virtual ~MenuletController();

        void setXMPPStatus(const std::string& message, Status status);
        void setUserNames(const std::vector<std::string>&);

        boost::signals2::signal<void ()> onRestartRequested;

    private:
        void update();

    private:
        Menulet* menulet;
        Status xmppStatus;
        std::string xmppStatusMessage;
        std::vector<std::string> linkLocalUsers;
};
