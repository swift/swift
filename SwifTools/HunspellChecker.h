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

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#include <SwifTools/SpellChecker.h>

class Hunspell;

namespace Swift {
    class HunspellChecker : public SpellChecker {
        public:
            HunspellChecker();
            virtual ~HunspellChecker();

            virtual bool isAutomaticallyDetectingLanguage();

            virtual void setActiveLanguage(const std::string& language);
            virtual std::string activeLanguage() const;
            virtual std::vector<std::string> supportedLanguages() const;

            virtual bool isCorrect(const std::string& word);
            virtual void getSuggestions(const std::string& word, std::vector<std::string>& list);
            virtual void checkFragment(const std::string& fragment, PositionPairList& misspelledPositions);

        private:
            struct Dictionary {
                std::string dicPath;
                std::string affPath;
            };

            std::unordered_map<std::string, Dictionary> detectedDictionaries() const;
            std::vector<std::string> hunspellDictionaryPaths() const;

        private:
            std::unique_ptr<Hunspell> speller_;
            boost::optional<std::string> activeLangauge_;

    };
}
