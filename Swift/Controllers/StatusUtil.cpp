/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/StatusUtil.h>

#include <cassert>

#include <Swift/Controllers/Intl.h>

namespace Swift {

std::string statusShowTypeToFriendlyName(StatusShow::Type type) {
    switch (type) {
        case StatusShow::Online: return QT_TRANSLATE_NOOP("", "Available");
        case StatusShow::FFC: return QT_TRANSLATE_NOOP("", "Available");
        case StatusShow::Away: return QT_TRANSLATE_NOOP("", "Away");
        case StatusShow::XA: return QT_TRANSLATE_NOOP("", "Away");
        case StatusShow::DND: return QT_TRANSLATE_NOOP("", "Busy");
        case StatusShow::None: return QT_TRANSLATE_NOOP("", "Offline");
    }
    assert(false);
    return "";
}

}
