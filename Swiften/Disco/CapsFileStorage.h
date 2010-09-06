/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/Disco/CapsStorage.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class CapsFileStorage : public CapsStorage {
		public:
			CapsFileStorage(const boost::filesystem::path& path);

			virtual DiscoInfo::ref getDiscoInfo(const String& hash) const;
			virtual void setDiscoInfo(const String& hash, DiscoInfo::ref discoInfo);

		private:
			boost::filesystem::path getCapsPath(const String& hash) const;

		private:
			boost::filesystem::path path;
	};
}
