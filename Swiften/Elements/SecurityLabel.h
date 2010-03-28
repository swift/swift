#ifndef SWIFTEN_SecurityLabel_H
#define SWIFTEN_SecurityLabel_H

#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class SecurityLabel : public Payload {
		public:
			SecurityLabel() {}

			const String& getDisplayMarking() const { return displayMarking_; }

			void setDisplayMarking(const String& displayMarking) { 
				displayMarking_ = displayMarking;
			}

			const String& getForegroundColor() const { 
				return foregroundColor_; 
			}

			void setForegroundColor(const String& foregroundColor) { 
				foregroundColor_ = foregroundColor;
			}

			const String& getBackgroundColor() const { 
				return backgroundColor_; 
			}

			void setBackgroundColor(const String& backgroundColor) { 
				backgroundColor_ = backgroundColor;
			}

			const String& getLabel() const { return label_; }

			void setLabel(const String& label) {
				label_ = label;
			}

			const std::vector<String>& getEquivalentLabels() const { return equivalentLabels_; }

			void addEquivalentLabel(const String& label) {
				equivalentLabels_.push_back(label);
			}

		private:
			String displayMarking_;
			String foregroundColor_;
			String backgroundColor_;
			String label_;
			std::vector<String> equivalentLabels_;
	};
}

#endif
