/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <Swift/QtUI/QtEmojisGrid.h>

namespace Swift {
    class QtEmoticonsGrid : public QtEmojisGrid {
        Q_OBJECT
    public:
        explicit QtEmoticonsGrid(const std::map<std::string, std::string>& emoticonsMap);
        ~QtEmoticonsGrid();

    };
}
