/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/HunspellChecker.h>

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <hunspell/hunspell.hxx>

#include <Swiften/Base/Log.h>

namespace Swift {

static std::vector<std::string> recursiveFileSearch(const std::vector<std::string>& paths, const boost::regex& regex) {
    std::vector<std::string> matches;

    for (auto& path : paths) {
        if (boost::filesystem::exists(path)) {
            boost::cmatch what;
            for (auto filename : boost::make_iterator_range(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator())) {
                if (boost::filesystem::is_regular_file(filename) && boost::regex_match(filename.path().c_str(), what, regex)) {
                    matches.push_back(filename.path().string());
                }
            }
        }
    }
    return matches;
}

HunspellChecker::HunspellChecker() {
}

HunspellChecker::~HunspellChecker() {
}

std::vector<std::string> HunspellChecker::hunspellDictionaryPaths() const {
    // The following list of paths comes from the source of the Hunspell command line tool.
    std::vector<std::string> paths = {
        "/usr/share/hunspell",
        "/usr/share/myspell",
        "/usr/share/myspell/dicts",
        "/Library/Spelling",
        "/opt/openoffice.org/basis3.0/share/dict/ooo",
        "/usr/lib/openoffice.org/basis3.0/share/dict/ooo",
        "/opt/openoffice.org2.4/share/dict/ooo",
        "/usr/lib/openoffice.org2.4/share/dict/ooo",
        "/opt/openoffice.org2.3/share/dict/ooo",
        "/usr/lib/openoffice.org2.3/share/dict/ooo",
        "/opt/openoffice.org2.2/share/dict/ooo",
        "/usr/lib/openoffice.org2.2/share/dict/ooo",
        "/opt/openoffice.org2.1/share/dict/ooo",
        "/usr/lib/openoffice.org2.1/share/dict/ooo",
        "/opt/openoffice.org2.0/share/dict/ooo",
        "/usr/lib/openoffice.org2.0/share/dict/ooo"
    };

    if (std::getenv("DICPATH")) {
        std::string dicpathEnvironment(std::getenv("DICPATH"));
        std::vector<std::string> dicpaths;
        boost::split(dicpaths,dicpathEnvironment,boost::is_any_of(":"));
        paths.insert(paths.begin(), dicpaths.begin(), dicpaths.end());
    }

    return paths;
}

bool HunspellChecker::isAutomaticallyDetectingLanguage() {
    return false;
}

void HunspellChecker::setActiveLanguage(const std::string& language) {
    auto dictionaries = detectedDictionaries();
    if (dictionaries.find(language) != dictionaries.end()) {
        SWIFT_LOG(debug) << "Initialized Hunspell with dic,aff files " << dictionaries[language].dicPath << " , " << dictionaries[language].affPath << std::endl;
        speller_ = std::unique_ptr<Hunspell>(new Hunspell(dictionaries[language].affPath.c_str(), dictionaries[language].dicPath.c_str()));
        activeLangauge_ = language;
    }
    else {
        SWIFT_LOG(warning) << "Unsupported language '" << language << "'" << std::endl;
    }
}

std::string HunspellChecker::activeLanguage() const {
    return activeLangauge_.get_value_or("");
}

std::vector<std::string> HunspellChecker::supportedLanguages() const {
    std::vector<std::string> languages;

    for (const auto& n : detectedDictionaries()) {
        languages.push_back(n.first);
    }

    return languages;
}

std::unordered_map<std::string, HunspellChecker::Dictionary> HunspellChecker::detectedDictionaries() const {
    std::unordered_map<std::string, HunspellChecker::Dictionary> dictionaries;

    auto dictionaryFiles = recursiveFileSearch(hunspellDictionaryPaths(), boost::regex(".*\\.dic$"));
    for (const auto& dictionary : dictionaryFiles) {
        std::string correspondingAffixPath = dictionary;
        boost::replace_last(correspondingAffixPath, ".dic", ".aff");
        if (boost::filesystem::is_regular_file(correspondingAffixPath)) {
            auto filenameWithoutExtension = boost::filesystem::basename(dictionary);
            dictionaries[filenameWithoutExtension] = {dictionary, correspondingAffixPath};
        }
    }

    return dictionaries;
}

bool HunspellChecker::isCorrect(const std::string& word) {
    if (speller_) {
        return speller_->spell(word.c_str());
    }
    else {
        return true;
    }
}

void HunspellChecker::getSuggestions(const std::string& word, std::vector<std::string>& list) {
    if (speller_) {
        char **suggestList = NULL;
        int words_returned = 0;
        if (!word.empty()) {
            words_returned = speller_->suggest(&suggestList, word.c_str());
            if (suggestList != NULL) {
                for (int i = 0; i < words_returned; ++i) {
                    list.push_back(suggestList[i]);
                    free(suggestList[i]);
                }
                free(suggestList);
            }
        }
    }
}

void HunspellChecker::checkFragment(const std::string& fragment, PositionPairList& misspelledPositions) {
    if (speller_) {
        if (!fragment.empty()) {
            parser_.check(fragment, misspelledPositions);
            for (PositionPairList::iterator it = misspelledPositions.begin(); it != misspelledPositions.end();) {
                if (isCorrect(fragment.substr(boost::get<0>(*it), boost::get<1>(*it) - boost::get<0>(*it)))) {
                    it = misspelledPositions.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
}

}
