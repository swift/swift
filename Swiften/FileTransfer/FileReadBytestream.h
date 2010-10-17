/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "Swiften/FileTransfer/ReadBytestream.h"

namespace Swift {
	class FileReadBytestream : public ReadBytestream {
		public:
			FileReadBytestream(const boost::filesystem::path& file);
			~FileReadBytestream();

			virtual ByteArray read(size_t size) ;
			virtual bool isFinished() const;

		private:
			boost::filesystem::path file;
			boost::filesystem::ifstream* stream;
	};
}
