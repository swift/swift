#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {

bool DiscoInfo::Identity::operator<(const Identity& other) const {
	if (category_ == other.category_) {
		if (type_ == other.type_) {
			if (lang_ == other.lang_) {
				return name_ < other.name_;
			}
			else {
				return lang_ < other.lang_;
			}
		}
		else {
			return type_ < other.type_;
		}
	}
	else {
		return category_ < other.category_;
	}
}

const std::string DiscoInfo::SecurityLabels = "urn:xmpp:sec-label:0";
}
