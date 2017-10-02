/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/EditlineTerminal.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>

#include <editline/readline.h>

#include <Swiften/Base/Platform.h>

#include <Sluift/Completer.h>

using namespace Swift;

static EditlineTerminal* globalInstance = nullptr;

static int completionStart = -1;
static int completionEnd = -1;

#if defined(SWIFTEN_PLATFORM_WINDOWS)
static char* getEmptyCompletions(const char*, int) {
#else
static int getEmptyCompletions(const char*, int) {
#endif
    return 0;
}

static char* getCompletions(const char*, int state) {
    rl_completion_append_character = 0;
#if RL_READLINE_VERSION >= 0x0600
    rl_completion_suppress_append = 1;
#endif

    static std::vector<std::string> completions;
    if (state == 0) {
        assert(globalInstance);
        completions.clear();
        if (globalInstance->getCompleter()) {
            completions = globalInstance->getCompleter()->getCompletions(rl_line_buffer, completionStart, completionEnd);
        }
    }
    if (boost::numeric_cast<size_t>(state) >= completions.size()) {
        return nullptr;
    }
    return strdup(completions[boost::numeric_cast<size_t>(state)].c_str());
}

static char** getAttemptedCompletions(const char* text, int start, int end) {
    completionStart = start;
    completionEnd = end;
    return rl_completion_matches(text, getCompletions);
}

EditlineTerminal& EditlineTerminal::getInstance() {
    static EditlineTerminal instance;
    globalInstance = &instance;
    return instance;
}

EditlineTerminal::EditlineTerminal() {
    rl_attempted_completion_function = getAttemptedCompletions;
    rl_completion_entry_function = getEmptyCompletions; // Fallback. Do nothing.
#if defined(SWIFTEN_PLATFORM_WINDOWS)
    // rl_basic_word_break is a cons char[] in MinGWEditLine.
    // This one seems to work, although it doesn't on OS X for some reason.
    rl_completer_word_break_characters = strdup(" \t\n.:+-*/><=;|&()[]{}");
#else
    rl_basic_word_break_characters = strdup(" \t\n.:+-*/><=;|&()[]{}");
#endif
}

EditlineTerminal::~EditlineTerminal() {
}

void EditlineTerminal::printError(const std::string& message) {
    std::cout << message << std::endl;
}

boost::optional<std::string> EditlineTerminal::readLine(const std::string& prompt) {
    const char* line = readline(prompt.c_str());
    return line ? std::string(line) : boost::optional<std::string>();
}

void EditlineTerminal::addToHistory(const std::string& line) {
#if defined(SWIFTEN_PLATFORM_WINDOWS)
    // MinGWEditLine copies the string, so this is safe
    add_history(const_cast<char*>(line.c_str()));
#else
    add_history(line.c_str());
#endif
}
