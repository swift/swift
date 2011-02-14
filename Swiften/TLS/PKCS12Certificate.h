/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PKCS12Certificate_H
#define SWIFTEN_PKCS12Certificate_H

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PKCS12Certificate {
		public:
			PKCS12Certificate() {}

			PKCS12Certificate(const std::string& filename, const std::string& password) : password_(password) {
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

			const std::string& getPassword() const {
				return password_;
			}

		private:
			ByteArray data_;
			std::string password_;
	};
}

#endif
