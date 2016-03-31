/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/Thread.h>

namespace Swift {

    Thread::Thread(const std::string& text, const std::string& parent) : text_(text), parent_(parent) {

    }

    Thread::~Thread() {

    }

    void Thread::setText(const std::string& text) {
        text_ = text;
    }

    const std::string& Thread::getText() const {
        return text_;
    }

    void Thread::setParent(const std::string& parent) {
        parent_ = parent;
    }

    const std::string& Thread::getParent() const {
        return parent_;
    }
}
