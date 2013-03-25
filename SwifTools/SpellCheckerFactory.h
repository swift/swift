/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>

#ifdef HAVE_HUNSPELL
#define HAVE_SPELLCHECKER
#elif defined(SWIFTEN_PLATFORM_MACOSX)
#define HAVE_SPELLCHECKER
#endif

namespace Swift {
	class SpellChecker;
	class SpellCheckerFactory {
		public:
			SpellCheckerFactory();
			SpellChecker* createSpellChecker(const std::string& dictFile);
	};
}
