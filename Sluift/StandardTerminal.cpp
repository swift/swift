/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/StandardTerminal.h>

#include <iostream>
#include <stdexcept>

#include <boost/optional.hpp>

using namespace Swift;

StandardTerminal::StandardTerminal() {
}

StandardTerminal::~StandardTerminal() {
}

void StandardTerminal::printError(const std::string& message) {
    std::cout << message << std::endl;
}

boost::optional<std::string> StandardTerminal::readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string input;
    if (!std::getline(std::cin, input)) {
        if (std::cin.eof()) {
            return boost::optional<std::string>();
        }
        throw std::runtime_error("Input error");
    }
    return input;
}

void StandardTerminal::addToHistory(const std::string&) {
}
