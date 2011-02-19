/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Translator.h>

#include <cassert>

namespace Swift {

struct DefaultTranslator : public Translator {
	virtual std::string translate(const std::string& text, const std::string&) {
		return text;
	}
} defaultTranslator;

Translator* Translator::translator = &defaultTranslator;

Translator::~Translator() {
}

void Translator::setInstance(Translator* t) {
	translator = t;
}

}
