/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/EmojiMapper.h>

#include <algorithm>
#include <string>
#include <unordered_map>

namespace Swift {

    //AUTO-GENERATED CONTENT
    <%= mapping %>

    std::vector<std::string> EmojiMapper::getCategories() {
        std::vector<std::string> categories = {
            "people",
            "nature",
            "food",
            "activity",
            "travel",
            "objects",
            "symbols",
            "flags"
        };

        for (const auto& keyValuePair : emojisInCategory) {
            if (std::find(std::begin(categories), std::end(categories), keyValuePair.first) == std::end(categories)) {
                categories.push_back(keyValuePair.first);
            }
        }
        return categories;
    }

    std::string EmojiMapper::shortnameToUnicode(const std::string& shortname) {
        auto unicodeSequenceIterator = shortnameUnicode.find(shortname);
        if (unicodeSequenceIterator != shortnameUnicode.end()) {
            return unicodeSequenceIterator->second;
        }
        else {
            return std::string();
        }
    }

    std::string EmojiMapper::unicodeToShortname(const std::string& unicode) {
        auto shortnameIterator = unicodeShortname.find(unicode);
        if (shortnameIterator != unicodeShortname.end()) {
            return shortnameIterator->second;
        }
        else {
            return std::string();
        }
    }

    std::vector<std::string> EmojiMapper::categoryNameToEmojis(const std::string& categoryName) {
        auto emojiIterator = emojisInCategory.find(categoryName);
        if (emojiIterator != emojisInCategory.end()) {
            return emojiIterator->second;
        }
        else {
            return std::vector<std::string>();
        }
    }

    std::string EmojiMapper::categoryToFlagshipUnicodeEmoji(const std::string& category) {
        if (category == "recent") {
            return shortnameToUnicode(":clock3:");
        } else if (category == "people") {
            return shortnameToUnicode(":smiley:");
        } else if (category == "nature" ) {
            return shortnameToUnicode(":dog:");
        } else if (category == "food") {
            return shortnameToUnicode(":apple:");
        } else if (category == "activity") {
            return shortnameToUnicode(":soccer:");
        } else if (category == "travel") {
            return shortnameToUnicode(":red_car:");
        } else if (category == "objects") {
            return shortnameToUnicode(":bulb:");
        } else if (category == "symbols") {
            return shortnameToUnicode(":heavy_division_sign:");
        } else if (category == "flags") {
            return shortnameToUnicode(":flag_white:");
        }
        return std::string();
    }
}
