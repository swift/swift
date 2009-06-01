#ifndef SWIFTEN_STRING_H
#define SWIFTEN_STRING_H

#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#define SWIFTEN_STRING_TO_CFSTRING(a) \
	CFStringCreateWithBytes(NULL, reinterpret_cast<const UInt8*>(a.getUTF8Data()), a.getUTF8Size(), kCFStringEncodingUTF8, false)

namespace Swift {
	class ByteArray;

	class String {
			friend class ByteArray;

		public:
			String() {}
			String(const char* data) : data_(data) {}
			String(const char* data, size_t len) : data_(data, len) {}
			String(const std::string& data) : data_(data) {}

			bool isEmpty() const { return data_.empty(); }

			const char* getUTF8Data() const { return data_.c_str(); }
			const std::string& getUTF8String() const { return data_; }
			std::string& getUTF8String() { return data_; }
			size_t getUTF8Size() const { return data_.size(); }
			std::vector<unsigned int> getUnicodeCodePoints() const;

			/**
			 * Returns the part before and after 'c'.
			 * If the given splitter does not occur in the string, the second
			 * component is the empty string.
			 */
			std::pair<String,String> getSplittedAtFirst(char c) const;

			size_t getLength() const;

			void removeAll(char c);

			void replaceAll(char c, const String& s);

			bool beginsWith(char c) const { 
				return data_.size() > 0 && data_[0] == c; 
			}

			bool beginsWith(const String& s) const {
				return data_.substr(0, s.data_.size()) == s;
			}

			bool endsWith(char c) const { 
				return data_.size() > 0 && data_[data_.size()-1] == c; 
			}

			String getSubstring(size_t begin, size_t end) const {
				return String(data_.substr(begin, end));
			}

			size_t find(char c) const {
				assert((c & 0x80) == 0);
				return data_.find(c);
			}

			size_t npos() const {
				return data_.npos;
			}

			friend String operator+(const String& a, const String& b) {
				return String(a.data_ + b.data_);
			}

			friend String operator+(const String& a, char b) {
				return String(a.data_ + b);
			}

			String& operator+=(const String& o) {
				data_ += o.data_;
				return *this;
			}

			String& operator+=(char c) {
				data_ += c;
				return *this;
			}

			friend bool operator>(const String& a, const String& b) {
				return a.data_ > b.data_;
			}

			friend bool operator<(const String& a, const String& b) {
				return a.data_ < b.data_;
			}

			friend bool operator!=(const String& a, const String& b) {
				return a.data_ != b.data_;
			}

			friend bool operator==(const String& a, const String& b) {
				return a.data_ == b.data_;
			}

			friend std::ostream& operator<<(std::ostream& os, const String& s) {
				os << s.data_;
				return os;
			}

		private:
			std::string data_;
	};
}

#endif
