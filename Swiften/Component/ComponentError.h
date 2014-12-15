/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
				UnexpectedElementError
			};

			ComponentError(Type type = UnknownError) : type_(type) {}

			Type getType() const { return type_; }

		private:
			Type type_;
	};
}
