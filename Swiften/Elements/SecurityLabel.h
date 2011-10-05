/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <string>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SecurityLabel : public Payload {
		public:
			typedef boost::shared_ptr<SecurityLabel> ref;
			SecurityLabel() {}

			const std::string& getDisplayMarking() const { return displayMarking_; }

			void setDisplayMarking(const std::string& displayMarking) { 
				displayMarking_ = displayMarking;
			}

			const std::string& getForegroundColor() const { 
				return foregroundColor_; 
			}

			void setForegroundColor(const std::string& foregroundColor) { 
				foregroundColor_ = foregroundColor;
			}

			const std::string& getBackgroundColor() const { 
				return backgroundColor_; 
			}

			void setBackgroundColor(const std::string& backgroundColor) { 
				backgroundColor_ = backgroundColor;
			}

			const std::string& getLabel() const { return label_; }

			void setLabel(const std::string& label) {
				label_ = label;
			}

			const std::vector<std::string>& getEquivalentLabels() const { return equivalentLabels_; }

			void addEquivalentLabel(const std::string& label) {
				equivalentLabels_.push_back(label);
			}

		private:
			std::string displayMarking_;
			std::string foregroundColor_;
			std::string backgroundColor_;
			std::string label_;
			std::vector<std::string> equivalentLabels_;
	};
}
