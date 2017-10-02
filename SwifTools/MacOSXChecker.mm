/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/MacOSXChecker.h>

#include <algorithm>
#include <cassert>

#include <boost/algorithm/string.hpp>

#include <AppKit/AppKit.h>

#include <SwifTools/Cocoa/CocoaUtil.h>

namespace Swift {

MacOSXChecker::MacOSXChecker() {
    NSSpellChecker* spellChecker = [NSSpellChecker sharedSpellChecker];
    [spellChecker setAutomaticallyIdentifiesLanguages:YES];
}

MacOSXChecker::~MacOSXChecker() {
}

bool MacOSXChecker::isCorrect(const std::string& /*word*/) {
    // No content since it doesn't seem to be used anywhere.
    return false;
}

bool MacOSXChecker::isAutomaticallyDetectingLanguage() {
    return true;
}

void MacOSXChecker::setActiveLanguage(const std::string& /*language*/) {
    assert(false);
}

std::string MacOSXChecker::activeLanguage() const {
    assert(false);
}
std::vector<std::string> MacOSXChecker::supportedLanguages() const {
    assert(false);
}

void MacOSXChecker::getSuggestions(const std::string& word, std::vector<std::string>& list) {
    NSSpellChecker* spellChecker = [NSSpellChecker sharedSpellChecker];
    NSString* wordString = [[NSString alloc] initWithUTF8String: word.c_str()];
    NSArray* suggestions = [spellChecker guessesForWordRange:NSMakeRange(0, [wordString length]) inString:wordString language:nil inSpellDocumentWithTag:0];
    for(unsigned int i = 0; i < [suggestions count]; ++i) {
        list.push_back(ns2StdString([suggestions objectAtIndex:i]));
    }
    [wordString release];
}

void MacOSXChecker::checkFragment(const std::string& fragment, PositionPairList& misspelledPositions) {
    NSSpellChecker* spellChecker = [NSSpellChecker sharedSpellChecker];
    size_t nextLocation = 0;
    NSRange range;
    NSString *fragmentString = [[NSString alloc] initWithUTF8String: fragment.c_str()];
    do {
        range = [spellChecker checkSpellingOfString:fragmentString startingAt:static_cast<long>(nextLocation)];
        if (range.location != NSNotFound) {
            if (range.location < nextLocation)
                break;
            misspelledPositions.push_back(PositionPair(range.location, range.location + range.length));
            nextLocation = range.location + range.length + 1;
        }
    } while (range.location != NSNotFound);
    [fragmentString release];
}

}
