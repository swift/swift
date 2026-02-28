/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/QtUI/QtResourceHelper.h>

namespace Swift {

QString statusShowTypeToIconPath(StatusShow::Type type) {
    QString iconString;
    switch (type) {
        case StatusShow::Online: iconString = "online";break;
        case StatusShow::Away: iconString = "away";break;
        case StatusShow::XA: iconString = "away";break;
        case StatusShow::FFC: iconString = "online";break;
        case StatusShow::DND: iconString = "dnd";break;
        case StatusShow::None: iconString = "offline";break;
    }
    return QString(":/icons/%1.png").arg(iconString);
}

}

