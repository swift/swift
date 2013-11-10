/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QString>

#include <Swiften/Elements/StatusShow.h>

namespace Swift {

QString statusShowTypeToIconPath(StatusShow::Type type);

}
