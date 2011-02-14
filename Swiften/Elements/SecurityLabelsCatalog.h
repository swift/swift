/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SecurityLabelsCatalog_H
#define SWIFTEN_SecurityLabelsCatalog_H

#include <vector>

#include "Swiften/JID/JID.h"
#include <string>
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/SecurityLabel.h"

namespace Swift {
	class SecurityLabelsCatalog : public Payload {
		public:
			SecurityLabelsCatalog(const JID& to = JID()) : to_(to) {}

			const std::vector<SecurityLabel>& getLabels() const {
				return labels_;
			}

			void addLabel(const SecurityLabel& label) {
				labels_.push_back(label);
			}

			const JID& getTo() const {
				return to_;
			}

			void setTo(const JID& to) {
				to_ = to;
			}

			const std::string& getName() const {
				return name_;
			}

			void setName(const std::string& name) {
				name_ = name;
			}

			const std::string& getDescription() const {
				return description_;
			}

			void setDescription(const std::string& description) {
				description_ = description;
			}

		private:
			JID to_;
			std::string name_;
			std::string description_;
			std::vector<SecurityLabel> labels_;
	};
}

#endif
