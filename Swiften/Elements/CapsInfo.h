/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class CapsInfo : public Payload {
		public:
			typedef boost::shared_ptr<CapsInfo> ref;

			CapsInfo(const String& node = "", const String& version = "", const String& hash = "sha-1") : node_(node), version_(version), hash_(hash) {}

			bool operator==(const CapsInfo& o) const {
				return o.node_ == node_ && o.version_ == version_ && o.hash_ == hash_;
			}

			bool operator<(const CapsInfo& o) const {
				if (o.node_ == node_) {
					if (o.version_ == version_) {
						return hash_ < o.hash_;
					}
					else {
						return version_ < o.version_;
					}
				}
				else {
					return node_ < o.node_;
				}
			}

			const String& getNode() const { return node_; }
			void setNode(const String& node) {
				node_ = node;
			}

			const String& getVersion() const { return version_; }
			void setVersion(const String& version) {
				version_ = version;
			}

			const String& getHash() const { return hash_; }
			void setHash(const String& hash) {
				hash_ = hash;
			}

		private:
			String node_;
			String version_;
			String hash_;
	};
}
