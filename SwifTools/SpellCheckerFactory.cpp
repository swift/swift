/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/filesystem/operations.hpp>

#include <SwifTools/SpellChecker.h>
#include <SwifTools/HunspellChecker.h>
#include <SwifTools/SpellCheckerFactory.h>
#include <Swiften/Base/Platform.h>

#ifdef HAVE_HUNSPELL
#include <hunspell/hunspell.hxx>
#elif defined(SWIFTEN_PLATFORM_MACOSX)
#include <SwifTools/MacOSXChecker.h>
#endif

namespace Swift {

SpellCheckerFactory::SpellCheckerFactory() {
}

#ifdef HAVE_HUNSPELL
SpellChecker* SpellCheckerFactory::createSpellChecker(const std::string& dictFile) {
    std::string affixFile(dictFile);
    boost::replace_all(affixFile, ".dic", ".aff");
    if ((boost::filesystem::exists(dictFile)) && (boost::filesystem::exists(affixFile))) {
        return new HunspellChecker(affixFile.c_str(), dictFile.c_str());
    }
    // If dictionaries don't exist disable the checker
    return NULL;
}
#elif defined(SWIFTEN_PLATFORM_MACOSX)
SpellChecker* SpellCheckerFactory::createSpellChecker(const std::string& /*dictFile*/) {
    return new MacOSXChecker();
}
#endif

}
