/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/cstdint.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class SHA256 {
		public:
			SHA256();

			SHA256& update(const std::vector<unsigned char>& data);
			std::vector<unsigned char> getHash() const;

			/**
			 * Equivalent of:
			 *	SHA256().update(data),getHash(), but slightly more efficient and
			 *	convenient.
			 */
			static ByteArray getHash(const ByteArray& data);
			static ByteArray getHash(const SafeByteArray& data);

			ByteArray operator()(const SafeByteArray& data) {
				return getHash(data);
			}

			ByteArray operator()(const ByteArray& data) {
				return getHash(data);
			}

		private:
			struct State {
					boost::uint64_t length;
					boost::uint32_t state[8], curlen;
					unsigned char buf[64];
			};

			static int init(State *md);
			static int process(State * md, const unsigned char *in, unsigned int inlen);
			static int compress(State *md, unsigned char *buf);
			static int done(State * md, unsigned char *out);

			template<typename Container> static	ByteArray getHashInternal(const Container& input);

		private:
			State state;
	};
}
