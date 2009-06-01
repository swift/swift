#ifndef SWIFTEN_BYTEARRAY_H
#define SWIFTEN_BYTEARRAY_H

#include <cstring>
#include <vector>
#include <iostream>

#include "Swiften/Base/String.h"

namespace Swift {
	class ByteArray
	{
		public:
			typedef std::vector<char>::const_iterator const_iterator;

			ByteArray() : data_() {}

			ByteArray(const String& s) : data_(s.getUTF8String().begin(), s.getUTF8String().end()) {}

			ByteArray(const char* c) {
				while (*c) {
					data_.push_back(*c);
					++c;
				}
			}

			ByteArray(const char* c, size_t n) {
				data_.resize(n);
				memcpy(&data_[0], c, n);
			}

			const char* getData() const {
				return &data_[0];
			}

			char* getData() {
				return &data_[0];
			}

			size_t getSize() const {
				return data_.size();
			}

			bool isEmpty() const {
				return data_.empty();
			}

			void resize(size_t size) {
				return data_.resize(size);
			}

			friend ByteArray operator+(const ByteArray& a, const ByteArray&b) {
				ByteArray result(a);
				result.data_.insert(result.data_.end(), b.data_.begin(), b.data_.end());
				return result;
			}

			friend bool operator==(const ByteArray& a, const ByteArray& b) {
				return a.data_ == b.data_;
			}


			const char& operator[](size_t i) const {
				return data_[i];
			}

			char& operator[](size_t i) {
				return data_[i];
			}

			const_iterator begin() const { 
				return data_.begin(); 
			}
		
			const_iterator end() const { 
				return data_.end(); 
			}

			String toString() const {
				return String(getData(), getSize());
			}

			void readFromFile(const String& file);

		private:
			std::vector<char> data_;
	};
}

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s);

#endif
