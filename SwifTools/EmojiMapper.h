/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Swift {
    class EmojiMapper {
        public:
            static std::string shortnameToUnicode(const std::string& shortname);
            static std::string unicodeToShortname(const std::string& unicode);
            static std::vector<std::string> categoryNameToEmojis(const std::string& category);
            static std::string categoryToFlagshipUnicodeEmoji(const std::string& category);
            static std::vector<std::string> getCategories();

        public:
            // \ref shortnameUnicode maps ASCII shortnames (e.g. joy, grin, smiley, …) to a emoji UTF-8 encoded unicode sequence.
            static const std::unordered_map<std::string, std::string> shortnameUnicode;
            // \ref unicodeShortname is the reverse mapping of \ref shortnameUnicode.
            static const std::unordered_map<std::string, std::string> unicodeShortname;
            // \ref emojisInCategory maps an emoji category name to a \ref std::vector of UTF-8 encoded unicode sequence strings.
            static const std::unordered_map<std::string, std::vector<std::string>> emojisInCategory;
    };
}
