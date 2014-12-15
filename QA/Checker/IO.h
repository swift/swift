/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s);
bool operator==(const Swift::ByteArray& a, const Swift::ByteArray& b);
std::ostream& operator<<(std::ostream& os, const Swift::SafeByteArray& s);
std::ostream& operator<<(std::ostream& os, const std::vector<int>& s);
std::ostream& operator<<(std::ostream& os, const std::vector<size_t>& s);
