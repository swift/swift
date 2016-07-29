/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Translator.h>

#include <cassert>

#include <Swiften/Base/DateTime.h>

namespace Swift {

static struct DefaultTranslator : public Translator {
    virtual std::string translate(const std::string& text, const std::string&) {
        return text;
    }

   virtual std::string ptimeToHumanReadableString(const boost::posix_time::ptime& time) {
        return dateTimeToLocalString(time);
   }
} defaultTranslator;

Translator* Translator::translator = &defaultTranslator;

Translator::~Translator() {
}

void Translator::setInstance(Translator* t) {
    translator = t;
}

}
