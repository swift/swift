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
}

bool AvatarFileStorage::hasAvatar(const std::string& hash) const { 
	return boost::filesystem::exists(getAvatarPath(hash));
}

void AvatarFileStorage::addAvatar(const std::string& hash, const ByteArray& avatar) {
	boost::filesystem::path avatarPath = getAvatarPath(hash);
	if (!boost::filesystem::exists(avatarPath.parent_path())) {
		try {
			boost::filesystem::create_directories(avatarPath.parent_path());
		}
		catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}
	boost::filesystem::ofstream file(avatarPath, boost::filesystem::ofstream::binary|boost::filesystem::ofstream::out);
	file.write(avatar.getData(), avatar.getSize());
	file.close();
}

boost::filesystem::path AvatarFileStorage::getAvatarPath(const std::string& hash) const {
	return path_ / hash;
}

ByteArray AvatarFileStorage::getAvatar(const std::string& hash) const {
	ByteArray data;
	data.readFromFile(getAvatarPath(hash).string());
	return data;
}


}
