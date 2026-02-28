/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmoticonsGrid.h>

#include <unordered_set>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojiCell.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

    QtEmoticonsGrid::QtEmoticonsGrid(const std::map<std::string, std::string>& emoticonsMap) : QtEmojisGrid() {
        std::unordered_set<std::string> usedEmoticons;
        for (const auto& emoticonPair : emoticonsMap) {
            if (usedEmoticons.find(emoticonPair.second) == usedEmoticons.end()) {
                usedEmoticons.insert(emoticonPair.second);

                auto filePath = P2QSTRING(emoticonPair.second);
                if (filePath.startsWith("qrc:/")) {
                    filePath.remove(0, 3);
                }
                auto icon = QIcon(filePath);
                auto text = P2QSTRING(emoticonPair.first);

                addEmoticon(icon, text);
            }
        }
    }

    QtEmoticonsGrid::~QtEmoticonsGrid() {

    }

}
