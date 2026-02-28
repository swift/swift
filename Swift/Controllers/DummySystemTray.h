/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/SystemTray.h>

namespace Swift {
    class DummySystemTray : public SystemTray {
    public:
        void setUnreadMessages(bool /*some*/) {}
        void setStatusType(StatusShow::Type /*type*/) {}
        void setConnecting() {}
    };
}
