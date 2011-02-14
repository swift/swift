/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class ServerError {
		public:
			enum Type {
				C2SPortConflict,
				C2SError,
				LinkLocalPortConflict,
				LinkLocalError
			};

			ServerError(Type type, const std::string& message = std::string()) :
					type(type), message(message) {
			}

			Type getType() const {
				return type;
			}

			const std::string& getMessage() const {
				return message;
			}

		private:
			Type type;
			std::string message;
	};
}
