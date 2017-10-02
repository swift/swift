/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/StatusShow.h>

namespace Swift {
    class SystemTray {
        public:
            virtual ~SystemTray(){}
            virtual void setUnreadMessages(bool some) = 0;
            virtual void setStatusType(StatusShow::Type type) = 0;
            virtual void setConnecting() = 0;
    };
}
