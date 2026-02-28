/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstddef>

namespace Swift {

// UTF-8 validation based on the description in https://tools.ietf.org/html/rfc3629#section-3 .
template <typename CharType>
bool UTF8IsValid(const CharType* data, size_t length) {
    bool isValid = true;
    const CharType* current = data;
    const CharType* end = data + length;
    while (isValid && (current < end)) {
        // one byte sequences
        if ((*current & 0x80) == 0x0) {
            current++;
            continue;
        }
        // longer byte sequences
        else {
            // two byte sequences
            if ((*current & 0xE0) == 0xC0) {
                current++;
                if ( (current < end) && ((*current & 0xC0) == 0x80) ) {
                    current++;
                    continue;
                }
            }
            // three byte sequences
            else if ((*current & 0xF0) == 0xE0) {
                current++;
                if ( ((current + 1) < end) && ((*current & 0xC0) == 0x80) ) {
                    current++;
                    if ((*current & 0xC0) == 0x80) {
                        current++;
                        continue;
                    }
                }
            }
            // four byte sequences
            else if ((*current & 0xF8) == 0xF0) {
                current++;
                if ( ((current + 2) < end) && ((*current & 0xC0) == 0x80) ) {
                    current++;
                    if ((*current & 0xC0) == 0x80) {
                        current++;
                        if ((*current & 0xC0) == 0x80) {
                            current++;
                            continue;
                        }
                    }
                }
            }
            // invalid sequences
            isValid = false;
        }
    }
    return isValid;
}

}
