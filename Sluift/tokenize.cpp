/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/tokenize.h>

#include <cctype>

#include <boost/tokenizer.hpp>

using namespace Swift;

namespace {
    struct LuaTokenizeFunctor {
        void reset() {
        }

        template<typename InputIterator, typename Token>
        bool operator()(InputIterator& next, InputIterator& end, Token& result) {
            while (next != end && std::isspace(*next)) {
                ++next;
            }
            if (next == end) {
                return false;
            }

            std::vector<char> token;
            char c = *next++;
            token.push_back(c);

            // String literal
            if (c == '\'' || c == '"') {
                char quote = c;
                bool inEscape = false;
                for (; next != end; ++next) {
                    c = *next;
                    token.push_back(c);
                    if (inEscape) {
                        inEscape = false;
                    }
                    else if (c == '\\') {
                        inEscape = true;
                    }
                    else if (c == quote) {
                        break;
                    }
                }
                if (next != end) {
                    ++next;
                }
            }
            // Identifier
            else if (std::isalpha(c) || c == '_') {
                while (next != end && (std::isalpha(*next) || *next == '_' || std::isdigit(*next))) {
                    token.push_back(*next);
                    ++next;
                }
            }
            // Digit
            else if (std::isdigit(c)) {
                while (next != end && !std::isspace(*next)) {
                    token.push_back(*next);
                    ++next;
                }
            }
            // Dots
            else if (c == '.') {
                while (next != end && *next == '.') {
                    token.push_back(*next);
                    ++next;
                }
            }

            result = Token(&token[0], token.size());
            return true;
        }
    };
}


std::vector<std::string> Lua::tokenize(const std::string& input) {
    boost::tokenizer<LuaTokenizeFunctor> tokenizer(input);
    return std::vector<std::string>(tokenizer.begin(), tokenizer.end());
}

