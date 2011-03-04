/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {

const std::string DiscoInfo::ChatStatesFeature = std::string("http://jabber.org/protocol/chatstates");
const std::string DiscoInfo::SecurityLabelsFeature = std::string("urn:xmpp:sec-label:0");
const std::string DiscoInfo::SecurityLabelsCatalogFeature = std::string("urn:xmpp:sec-label:catalog:2");
const std::string DiscoInfo::JabberSearchFeature = std::string("jabber:iq:search");


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

}
