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

class SWIFTEN_API SOCKS5BytestreamServerResourceUser {
    public:
        SOCKS5BytestreamServerResourceUser(SOCKS5BytestreamServerManager* s5bServerManager);
        ~SOCKS5BytestreamServerResourceUser();

        bool isInitialized() const;

        boost::signals2::signal<void (bool /* success */)> onSuccessfulInitialized;

    private:
        void handleServerManagerInitialized(bool successfulInitialize);

    private:
        SOCKS5BytestreamServerManager* s5bServerManager_;
        boost::signals2::scoped_connection onInitializedConnection_;
};

}
