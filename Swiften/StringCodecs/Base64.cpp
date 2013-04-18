/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// Code copied & adapted from LibTomCrypt by Tom St Denis

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>

#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/Algorithm.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wconversion"


static const char *codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

namespace {
	template<typename TargetType, typename SourceType>
 	TargetType base64Encode(const SourceType& in) {
 		TargetType out;

 		size_t i;
 		unsigned int leven = 3*(in.size() / 3);
 		for (i = 0; i < leven; i += 3) {
 			out.push_back(codes[(in[i] >> 2) & 0x3F]);
 			out.push_back(codes[(((in[i] & 3) << 4) + (in[i+1] >> 4)) & 0x3F]);
 			out.push_back(codes[(((in[i+1] & 0xf) << 2) + (in[i+2] >> 6)) & 0x3F]);
 			out.push_back(codes[in[i+2] & 0x3F]);
 		}
 		if (i < in.size()) {
 			unsigned int a = in[i];
 			unsigned int b = (i+1 < in.size()) ? in[i+1] : 0;

 			out.push_back(codes[(a >> 2) & 0x3F]);
 			out.push_back(codes[(((a & 3) << 4) + (b >> 4)) & 0x3F]);
 			out.push_back((i+1 < in.size()) ? codes[(((b & 0xf) << 2)) & 0x3F] : '=');
 			out.push_back('=');
 		}
 		return out;
 	}
}

static const unsigned char map[256] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
	37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255 };

namespace Swift {
	std::string Base64::encode(const ByteArray &s) {
		return base64Encode<std::string, ByteArray>(s);
	}

	SafeByteArray Base64::encode(const SafeByteArray &s) {
		return base64Encode<SafeByteArray, SafeByteArray>(s);
	}

	ByteArray Base64::decode(const std::string& input) {
		std::string inputWithoutNewlines(input);
		erase(inputWithoutNewlines, '\n');

		const std::string& s = inputWithoutNewlines;
		ByteArray out;

		int g = 3;
		size_t y, t;
		for (size_t x = y = t = 0; x < s.size(); ++x) {
			unsigned char c = map[s[x]&0xFF];
			if (c == 255) {
				continue;
			}
		 /* the final = symbols are read and used to trim the remaining bytes */
			if (c == 254) { 
				c = 0; 
				/* prevent g < 0 which would potentially allow an overflow later */
				if (--g < 0) {
					return ByteArray();
				}
			} else if (g != 3) {
				/* we only allow = to be at the end */
				return ByteArray();
			}

			t = (t<<6)|c;

			if (++y == 4) {
				out.push_back((unsigned char)((t>>16)&255));
				if (g > 1) out.push_back((unsigned char)((t>>8)&255));
				if (g > 2) out.push_back((unsigned char)(t&255));
				y = t = 0;
			}
		}
		if (y != 0) {
			return ByteArray();
		}
		return out;
	}
}
