/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {
	class FileWriteBytestream : public WriteBytestream {
		public:
			FileWriteBytestream(const boost::filesystem::path& file);
			~FileWriteBytestream();

			virtual void write(const std::vector<unsigned char>&);
			void close();

		private:
			boost::filesystem::path file;
			boost::filesystem::ofstream* stream;
	};
}
