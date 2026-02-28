/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Sluift/Terminal.h>

namespace Swift {
    class StandardTerminal : public Terminal {
        public:
            StandardTerminal();
            virtual ~StandardTerminal() override;

            virtual boost::optional<std::string> readLine(const std::string& prompt) override;
            virtual void printError(const std::string& message) override;
            virtual void addToHistory(const std::string& command) override;
    };
}
