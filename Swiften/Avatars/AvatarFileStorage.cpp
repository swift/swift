/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Avatars/AvatarFileStorage.h>

#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

AvatarFileStorage::AvatarFileStorage(const boost::filesystem::path& avatarsDir, const boost::filesystem::path& avatarsFile) : avatarsDir(avatarsDir), avatarsFile(avatarsFile) {
	if (boost::filesystem::exists(avatarsFile)) {
		boost::property_tree::ptree tree;
		try {
			boost::property_tree::xml_parser::read_xml(avatarsFile.string(), tree);
		}
		catch (const boost::property_tree::xml_parser::xml_parser_error& e) {
			std::cerr << "Error reading avatars file: " << e.filename() << ":" << e.line() << ": " << e.message() << std::endl;
		}
		foreach(const boost::property_tree::ptree::value_type &v, tree.get_child("avatars")) {
			try {
				JID jid(v.second.get<std::string>("jid"));
				std::string hash(v.second.get<std::string>("hash"));
				if (jid.isValid()) {
					jidAvatars.insert(std::make_pair(jid, hash));
				}
			}
			catch (const boost::property_tree::ptree_error& e) {
				std::cerr << "Invalid avatar value: " << e.what() << std::endl;
			}
		}
	}
}

bool AvatarFileStorage::hasAvatar(const std::string& hash) const { 
	return boost::filesystem::exists(getAvatarPath(hash));
}

void AvatarFileStorage::addAvatar(const std::string& hash, const ByteArray& avatar) {
	assert(Hexify::hexify(SHA1::getHash(avatar)) == hash);

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
	file.write(reinterpret_cast<const char*>(avatar.getData()), avatar.getSize());
	file.close();
}

boost::filesystem::path AvatarFileStorage::getAvatarPath(const std::string& hash) const {
	return avatarsDir / hash;
}

ByteArray AvatarFileStorage::getAvatar(const std::string& hash) const {
	ByteArray data;
	data.readFromFile(getAvatarPath(hash).string());
	return data;
}

void AvatarFileStorage::setAvatarForJID(const JID& jid, const std::string& hash) {
	std::pair<JIDAvatarMap::iterator, bool> r = jidAvatars.insert(std::make_pair(jid, hash));
	if (r.second) {
		saveJIDAvatars();
	}
	else if (r.first->second != hash) {
		r.first->second = hash;
		saveJIDAvatars();
	}
}

std::string AvatarFileStorage::getAvatarForJID(const JID& jid) const {
	JIDAvatarMap::const_iterator i = jidAvatars.find(jid);
	return i == jidAvatars.end() ? "" : i->second;
}

void AvatarFileStorage::saveJIDAvatars() {
	boost::property_tree::ptree tree;
	for (JIDAvatarMap::const_iterator i = jidAvatars.begin(); i != jidAvatars.end(); ++i) {
		boost::property_tree::ptree entry;
		entry.put("jid", i->first.toString());
		entry.put("hash", i->second);
		tree.add_child("avatars.avatar", entry);
	}
	try {
		boost::property_tree::xml_parser::write_xml(avatarsFile.string(), tree);
	}
	catch (const boost::property_tree::xml_parser::xml_parser_error& e) {
		std::cerr << "Error writing avatars file: " << e.filename() << ": " << e.message() << std::endl;
	}
}

}
