/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class ComponentError {
		public:
			enum Type {
				UnknownError,
				ConnectionError,
				ConnectionReadError,
				ConnectionWriteError,
				XMLError,
				AuthenticationFailedError,
				UnexpectedElementError,
			};

			ComponentError(Type type = UnknownError) : type_(type) {}

			Type getType() const { return type_; }

		private:
			Type type_;
	};
}
