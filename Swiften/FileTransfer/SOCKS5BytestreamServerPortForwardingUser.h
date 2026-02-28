/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>

namespace Swift {

class SOCKS5BytestreamServerManager;

class SWIFTEN_API SOCKS5BytestreamServerPortForwardingUser {
    public:
        SOCKS5BytestreamServerPortForwardingUser(SOCKS5BytestreamServerManager* s5bServerManager);
        ~SOCKS5BytestreamServerPortForwardingUser();

        bool isForwardingSetup() const;

        boost::signals2::signal<void (bool /* success */)> onSetup;

    private:
        void handleServerManagerPortForwardingSetup(bool successful);

    private:
        SOCKS5BytestreamServerManager* s5bServerManager_;
        boost::signals2::scoped_connection onPortForwardingSetupConnection_;
};

}
