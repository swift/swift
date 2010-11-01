/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class FileTransferError {
		public:
			enum Type {
				UnknownError,
				PeerError,
				ReadError,
				ClosedError,
			};

			FileTransferError(Type type = UnknownError) : type(type) {}

			Type getType() const {
				return type;
			}

		private:
			Type type;
	};
}
