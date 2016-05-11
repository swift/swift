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
SpellChecker* SpellCheckerFactory::createSpellChecker() {
    return new HunspellChecker();
}
#elif defined(SWIFTEN_PLATFORM_MACOSX)
SpellChecker* SpellCheckerFactory::createSpellChecker() {
    return new MacOSXChecker();
}
#endif

}
