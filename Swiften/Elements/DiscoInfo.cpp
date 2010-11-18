/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {

const String DiscoInfo::ChatStatesFeature = String("http://jabber.org/protocol/chatstates");
const String DiscoInfo::SecurityLabelsFeature = String("urn:xmpp:sec-label:0");
const String DiscoInfo::JabberSearchFeature = String("jabber:iq:search");


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
