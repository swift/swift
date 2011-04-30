/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class CapsInfo : public Payload {
		public:
			typedef boost::shared_ptr<CapsInfo> ref;

			CapsInfo(const std::string& node = "", const std::string& version = "", const std::string& hash = "sha-1") : node_(node), version_(version), hash_(hash) {}

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

			const std::string& getNode() const { return node_; }
			void setNode(const std::string& node) {
				node_ = node;
			}

			const std::string& getVersion() const { return version_; }
			void setVersion(const std::string& version) {
				version_ = version;
			}

			const std::string& getHash() const { return hash_; }
			void setHash(const std::string& hash) {
				hash_ = hash;
			}

		private:
			std::string node_;
			std::string version_;
			std::string hash_;
	};
}
