#ifndef SWIFTEN_SecurityLabelsCatalog_H
#define SWIFTEN_SecurityLabelsCatalog_H

#include <vector>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
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

			const String& getName() const {
				return name_;
			}

			void setName(const String& name) {
				name_ = name;
			}

			const String& getDescription() const {
				return description_;
			}

			void setDescription(const String& description) {
				description_ = description;
			}

		private:
			JID to_;
			String name_;
			String description_;
			std::vector<SecurityLabel> labels_;
	};
}

#endif
