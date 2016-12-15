/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional/optional.hpp>

namespace Swift {
    class Completer;

    class Terminal {
        public:
            Terminal();
            virtual ~Terminal();

            Completer* getCompleter() const {
                return completer;
            }

            void setCompleter(Completer* completer) {
                this->completer = completer;
            }

            virtual boost::optional<std::string> readLine(const std::string& prompt) = 0;
            virtual void addToHistory(const std::string& command) = 0;
            virtual void printError(const std::string& message) = 0;

        private:
            Completer* completer;
    };
}
