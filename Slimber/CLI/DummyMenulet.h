/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Slimber/Menulet.h>

class DummyMenulet : public Menulet {
    public:
        DummyMenulet() {
        }

        void clear() {
        }

        void addItem(const std::string&, const std::string&) {
        }

        void addAboutItem() {
        }

        void addRestartItem() {
        }

        void addExitItem() {
        }

        void addSeparator() {
        }

        void setIcon(const std::string&) {
        }
};
