/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <algorithm>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Elements/Form.h"

namespace Swift {
	class DiscoInfo : public Payload, public Shared<DiscoInfo> {
		public:
			const static std::string SecurityLabels;
			class Identity {
				public:
					Identity(const String& name, const String& category = "client", const String& type = "pc", const String& lang = "") : name_(name), category_(category), type_(type), lang_(lang) {
					}

					const String& getCategory() const {
						return category_;
					}
					
					const String& getType() const {
						return type_;
					}

					const String& getLanguage() const {
						return lang_;
					}

					const String& getName() const {
						return name_;
					}

					// Sorted according to XEP-115 rules
					bool operator<(const Identity& other) const;

				private:
					String name_;
					String category_;
					String type_;
					String lang_;
			};

			DiscoInfo() {
			}

			const String& getNode() const {
				return node_;
			}

			void setNode(const String& node) {
				node_ = node;
			}

			const std::vector<Identity>& getIdentities() const {
				return identities_;
			}

			void addIdentity(const Identity& identity) {
				identities_.push_back(identity);
			}

			const std::vector<String>& getFeatures() const {
				return features_;
			}

			void addFeature(const String& feature) {
				features_.push_back(feature);
			}

			bool hasFeature(const String& feature) const {
				return std::find(features_.begin(), features_.end(), feature) != features_.end();
			}

			void addExtension(Form::ref form) {
				extensions_.push_back(form);
			}

			const std::vector<Form::ref> getExtensions() const {
				return extensions_;
			}

		private:
			String node_;
			std::vector<Identity> identities_;
			std::vector<String> features_;
			std::vector<Form::ref> extensions_;
	};
}
