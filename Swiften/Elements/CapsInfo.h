/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_CapsInfo_H
#define SWIFTEN_CapsInfo_H

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class CapsInfo : public Payload {
		public:
			CapsInfo(const String& node, const String& version, const String& hash = "sha-1") : node_(node), version_(version), hash_(hash) {}

			const String& getNode() const { return node_; }
			const String& getVersion() const { return version_; }
			const String& getHash() const { return hash_; }

		private:
			String node_;
			String version_;
			String hash_;
	};
}

#endif
