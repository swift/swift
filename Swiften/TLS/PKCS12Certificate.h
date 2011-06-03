/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class PKCS12Certificate {
		public:
			PKCS12Certificate() {}

			PKCS12Certificate(const std::string& filename, const SafeByteArray& password) : password_(password) {
				readByteArrayFromFile(data_, filename);
			}

			bool isNull() const {
				return data_.empty();
			}

			const ByteArray& getData() const {
				return data_;
			}

			void setData(const ByteArray& data) {
				data_ = data;
			}

			const SafeByteArray& getPassword() const {
				return password_;
			}

		private:
			ByteArray data_;
			SafeByteArray password_;
	};
}
