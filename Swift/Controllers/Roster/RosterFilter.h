/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Roster/RosterItem.h>

namespace Swift {

class RosterFilter {
    public:
        virtual ~RosterFilter() {}
        virtual bool operator() (RosterItem* item) const = 0;
};

}


