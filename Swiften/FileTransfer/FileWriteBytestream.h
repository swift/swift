/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {
    class SWIFTEN_API FileWriteBytestream : public WriteBytestream {
        public:
            FileWriteBytestream(const boost::filesystem::path& file);
            virtual ~FileWriteBytestream();

            virtual bool write(const std::vector<unsigned char>&);
            virtual void close();

        private:
            boost::filesystem::path file;
            boost::filesystem::ofstream* stream;
    };
}
