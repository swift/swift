/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_STANZAS_VERSION_H
#define SWIFTEN_STANZAS_VERSION_H

#include <string>
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class Version : public Payload
	{
		public:
			Version(const std::string& name = "", const std::string& version = "", const std::string& os = "") : name_(name), version_(version), os_(os) { }

			const std::string& getName() const { return name_; }
			const std::string& getVersion() const { return version_; }
			const std::string& getOS() const { return os_; }

		private:
			std::string name_;
			std::string version_;
			std::string os_;
	};
}

#endif
