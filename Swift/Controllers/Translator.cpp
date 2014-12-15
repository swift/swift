/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Translator.h>

#include <cassert>

namespace Swift {

static struct DefaultTranslator : public Translator {
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
