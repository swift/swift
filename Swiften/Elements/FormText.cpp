/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <Swiften/Elements/FormText.h>

namespace Swift {

FormText::FormText() {
}

FormText::~FormText() {
}

void FormText::setTextString(const std::string& text) {
    text_ = text;
}

const std::string& FormText::getTextString() const {
    return text_;
}

}
