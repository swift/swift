#ifndef SWIFTEN_PKCS12Certificate_H
#define SWIFTEN_PKCS12Certificate_H

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PKCS12Certificate {
		public:
			PKCS12Certificate() {}

			PKCS12Certificate(const String& filename, const String& password) : password_(password) {
				data_.readFromFile(filename);
			}

			bool isNull() const {
				return data_.isEmpty();
			}

			const ByteArray& getData() const {
				return data_;
			}

			void setData(const ByteArray& data) {
				data_ = data;
			}

			const String& getPassword() const {
				return password_;
			}

		private:
			ByteArray data_;
			String password_;
	};
}

#endif
