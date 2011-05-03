/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Disco/CapsInfoGenerator.h>

#include <algorithm>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Base64.h>

namespace {
	bool compareFields(Swift::FormField::ref f1, Swift::FormField::ref f2) {
		return f1->getName() < f2->getName();
	}
}

namespace Swift {

CapsInfoGenerator::CapsInfoGenerator(const std::string& node) : node_(node) {
}

CapsInfo CapsInfoGenerator::generateCapsInfo(const DiscoInfo& discoInfo) const {
	std::string serializedCaps;

	std::vector<DiscoInfo::Identity> identities(discoInfo.getIdentities());
	std::sort(identities.begin(), identities.end());
	foreach (const DiscoInfo::Identity& identity, identities) {
		serializedCaps += identity.getCategory() + "/" + identity.getType() + "/" + identity.getLanguage() + "/" + identity.getName() + "<";
	}

	std::vector<std::string> features(discoInfo.getFeatures());
	std::sort(features.begin(), features.end());
	foreach (const std::string& feature, features) {
		serializedCaps += feature + "<";
	}

	foreach(Form::ref extension, discoInfo.getExtensions()) {
		serializedCaps += extension->getFormType() + "<";
		std::vector<FormField::ref> fields(extension->getFields());
		std::sort(fields.begin(), fields.end(), &compareFields);
		foreach(FormField::ref field, fields) {
			if (field->getName() == "FORM_TYPE") {
				continue;
			}
			serializedCaps += field->getName() + "<";
			std::vector<std::string> values(field->getRawValues());
			std::sort(values.begin(), values.end());
			foreach(const std::string& value, values) {
				serializedCaps += value + "<";
			}
		}
	}

	std::string version(Base64::encode(SHA1::getHash(createByteArray(serializedCaps))));
	return CapsInfo(node_, version, "sha-1");
}

}
