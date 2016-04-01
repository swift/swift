/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <Slimber/ServerError.h>

namespace Swift {
    class DNSSDQuerier;
    class LinkLocalServiceBrowser;
    class VCardCollection;
    class Server;
    class EventLoop;
}

class MenuletController;
class Menulet;

class MainController {
    public:
        MainController(Menulet* menulet, Swift::EventLoop* eventLoop);
        virtual ~MainController();

    private:
        void handleSelfConnected(bool b);
        void handleServicesChanged();
        void handleServerStopped(boost::optional<Swift::ServerError> error);
        void handleRestartRequested();

        void start();
        void stop();

    private:
        std::shared_ptr<Swift::DNSSDQuerier> dnsSDQuerier;
        Swift::LinkLocalServiceBrowser* linkLocalServiceBrowser;
        Swift::VCardCollection* vCardCollection;
        Swift::Server* server;
        MenuletController* menuletController;
};
