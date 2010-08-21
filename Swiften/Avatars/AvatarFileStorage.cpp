/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/AvatarFileStorage.h"

#include <iostream>
#include <boost/filesystem/fstream.hpp>

namespace Swift {

AvatarFileStorage::AvatarFileStorage(const boost::filesystem::path& path) : path_(path) {
	std::cerr << "Creating avatar file storage: " << path.string() << std::endl;
	try {
		boost::filesystem::create_directory(path_);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

bool AvatarFileStorage::hasAvatar(const String& hash) const { 
	return boost::filesystem::exists(getAvatarPath(hash));
}

void AvatarFileStorage::addAvatar(const String& hash, const ByteArray& avatar) {
	boost::filesystem::ofstream file(getAvatarPath(hash), boost::filesystem::ofstream::binary|boost::filesystem::ofstream::out);
	file.write(avatar.getData(), avatar.getSize());
	file.close();
}

boost::filesystem::path AvatarFileStorage::getAvatarPath(const String& hash) const {
	return path_ / hash.getUTF8String();
}

}
