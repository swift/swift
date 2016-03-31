/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

namespace Swift {
    class Translator {
        public:
            virtual ~Translator();

            virtual std::string translate(const std::string& text, const std::string& context) = 0;

            static void setInstance(Translator* translator);

            static Translator* getInstance() {
                return translator;
            }

        private:
            static Translator* translator;
    };
}
