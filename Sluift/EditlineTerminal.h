/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Sluift/Terminal.h>

namespace Swift {
    class EditlineTerminal : public Terminal {
        public:
            static EditlineTerminal& getInstance();

        private:
            EditlineTerminal();
            virtual ~EditlineTerminal() override;

            virtual boost::optional<std::string> readLine(const std::string& prompt) override;
            virtual void printError(const std::string& message) override;
            virtual void addToHistory(const std::string& command) override;
    };
}
