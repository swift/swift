/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Sluift/LuaElementConvertors.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#ifdef HAVE_ITUNES
#include <Sluift/ITunesInterface.h>
#endif
#include <signal.h>

namespace Swift {
    struct SluiftGlobals {
        SluiftGlobals() :
            networkFactories(&eventLoop),
            coreLibIndex(-1),
            moduleLibIndex(-1),
            interruptRequested(0) {}

        LuaElementConvertors elementConvertor;
        SimpleEventLoop eventLoop;
        BoostNetworkFactories networkFactories;
        PlatformTLSFactories tlsFactories;
        int coreLibIndex;
        int moduleLibIndex;
        sig_atomic_t interruptRequested;
#ifdef HAVE_ITUNES
        ITunesInterface iTunes;
#endif
    };
}
