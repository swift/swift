/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <algorithm>

#include "Swiften/Elements/Payload.h"
#include <string>

#include "Swiften/Elements/Form.h"

namespace Swift {
	class DiscoInfo : public Payload {
		public:
			typedef boost::shared_ptr<DiscoInfo> ref;

			static const std::string ChatStatesFeature;
			static const std::string SecurityLabelsFeature;
			static const std::string SecurityLabelsCatalogFeature;
			static const std::string JabberSearchFeature;

			class Identity {
				public:
					Identity(const std::string& name, const std::string& category = "client", const std::string& type = "pc", const std::string& lang = "") : name_(name), category_(category), type_(type), lang_(lang) {
					}

					const std::string& getCategory() const {
						return category_;
					}
					
					const std::string& getType() const {
						return type_;
					}

					const std::string& getLanguage() const {
						return lang_;
					}

					const std::string& getName() const {
						return name_;
					}

					// Sorted according to XEP-115 rules
					bool operator<(const Identity& other) const;

				private:
					std::string name_;
					std::string category_;
					std::string type_;
					std::string lang_;
			};

			DiscoInfo() {
			}

			const std::string& getNode() const {
				return node_;
			}

			void setNode(const std::string& node) {
				node_ = node;
			}

			const std::vector<Identity>& getIdentities() const {
				return identities_;
			}

			void addIdentity(const Identity& identity) {
				identities_.push_back(identity);
			}

			const std::vector<std::string>& getFeatures() const {
				return features_;
			}

			void addFeature(const std::string& feature) {
				features_.push_back(feature);
			}

			bool hasFeature(const std::string& feature) const {
				return std::find(features_.begin(), features_.end(), feature) != features_.end();
			}

			void addExtension(Form::ref form) {
				extensions_.push_back(form);
			}

			const std::vector<Form::ref> getExtensions() const {
				return extensions_;
			}

		private:
			std::string node_;
			std::vector<Identity> identities_;
			std::vector<std::string> features_;
			std::vector<Form::ref> extensions_;
	};
}
