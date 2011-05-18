/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/Base/SafeAllocator.h>

namespace Swift {
	class SafeString {
		public:
			typedef std::vector<char, SafeAllocator<char> > Data;
			typedef Data::iterator Iterator;
			typedef Data::const_iterator ConstIterator;

			SafeString() {
			}

			SafeString(const std::string& s) : data(s.begin(), s.end()) {
			}

			SafeString(const char*);


			std::string toString() const {
				return data.empty() ? std::string() : std::string(&data[0], data.size());
			}

			void resize(size_t n) {
				data.resize(n);
			}

			char& operator[](size_t n) {
				return data[n];
			}

			Iterator begin() {
				return data.begin();
			}

			Iterator end() {
				return data.end();
			}

			ConstIterator begin() const {
				return data.begin();
			}

			ConstIterator end() const {
				return data.end();
			}

			size_t size() const {
				return data.size();
			}

		private:
			std::vector<char, SafeAllocator<char> > data;
	};
};
