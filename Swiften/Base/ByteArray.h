/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cstring>
#include <vector>
#include <iostream>

#include <string>

namespace Swift {
	class ByteArray
	{
		public:
			typedef std::vector<unsigned char>::const_iterator const_iterator;

			ByteArray() : data_() {}

			ByteArray(const std::string& s) : data_(s.begin(), s.end()) {}

			ByteArray(const char* c) {
				while (*c) {
					data_.push_back(*c);
					++c;
				}
			}

			ByteArray(const char* c, size_t n) {
				if (n > 0) {
					data_.resize(n);
					memcpy(&data_[0], c, n);
				}
			}

			ByteArray(const unsigned char* c, size_t n) {
				if (n > 0) {
					data_.resize(n);
					memcpy(&data_[0], c, n);
				}
			}

			ByteArray(const std::vector<unsigned char>& data) : data_(data) {
			}

			const unsigned char* getData() const {
				return data_.empty() ? NULL : &data_[0];
			}

			unsigned char* getData() {
				return data_.empty() ? NULL : &data_[0];
			}
			
			const std::vector<unsigned char>& getVector() const {
				return data_;
			}

			std::vector<unsigned char>& getVector() {
				return data_;
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

			void resize(size_t size, char c) {
				return data_.resize(size, c);
			}

			friend ByteArray operator+(const ByteArray& a, const ByteArray&b) {
				ByteArray result(a);
				result.data_.insert(result.data_.end(), b.data_.begin(), b.data_.end());
				return result;
			}

			friend ByteArray operator+(const ByteArray& a, char b) {
				ByteArray x;
				x.resize(1);
				x[0] = b;
				return a + x;
			}

			ByteArray& operator+=(const ByteArray& b) {
				data_.insert(data_.end(), b.data_.begin(), b.data_.end());
				return *this;
			}

			ByteArray& operator+=(char c) {
				data_.push_back(c);
				return *this;
			}

			friend bool operator==(const ByteArray& a, const ByteArray& b) {
				return a.data_ == b.data_;
			}


			const unsigned char& operator[](size_t i) const {
				return data_[i];
			}

			unsigned char& operator[](size_t i) {
				return data_[i];
			}

			const_iterator begin() const { 
				return data_.begin(); 
			}
		
			const_iterator end() const { 
				return data_.end(); 
			}

			std::string toString() const {
				return std::string(reinterpret_cast<const char*>(getData()), getSize());
			}

			void readFromFile(const std::string& file);

			void clear() {
				data_.clear();
			}

		private:
			std::vector<unsigned char> data_;
	};
}

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s);
