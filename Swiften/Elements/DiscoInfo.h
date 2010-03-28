#ifndef SWIFTEN_DiscoInfo_H
#define SWIFTEN_DiscoInfo_H

#include <vector>
#include <algorithm>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class DiscoInfo : public Payload {
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

			const std::vector<Identity> getIdentities() const {
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

		private:
			String node_;
			std::vector<Identity> identities_;
			std::vector<String> features_;
	};
}

#endif
