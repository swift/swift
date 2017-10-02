/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/CapsInfoGenerator.h>

#include <algorithm>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/FormField.h>
#include <Swiften/StringCodecs/Base64.h>

namespace {
    bool compareFields(Swift::FormField::ref f1, Swift::FormField::ref f2) {
        return f1->getName() < f2->getName();
    }
}

namespace Swift {

CapsInfoGenerator::CapsInfoGenerator(const std::string& node, CryptoProvider* crypto) : node_(node), crypto_(crypto) {
}

CapsInfo CapsInfoGenerator::generateCapsInfo(const DiscoInfo& discoInfo) const {
    std::string serializedCaps;

    std::vector<DiscoInfo::Identity> identities(discoInfo.getIdentities());
    std::sort(identities.begin(), identities.end());
    for (const auto& identity : identities) {
        serializedCaps += identity.getCategory() + "/" + identity.getType() + "/" + identity.getLanguage() + "/" + identity.getName() + "<";
    }

    std::vector<std::string> features(discoInfo.getFeatures());
    std::sort(features.begin(), features.end());
    for (const auto& feature : features) {
        serializedCaps += feature + "<";
    }

    for (const auto& extension : discoInfo.getExtensions()) {
        serializedCaps += extension->getFormType() + "<";
        std::vector<FormField::ref> fields(extension->getFields());
        std::sort(fields.begin(), fields.end(), &compareFields);
        for (const auto& field : fields) {
            if (field->getName() == "FORM_TYPE") {
                continue;
            }
            serializedCaps += field->getName() + "<";
            std::vector<std::string> values(field->getValues());
            std::sort(values.begin(), values.end());
            for (const auto& value : values) {
                serializedCaps += value + "<";
            }
        }
    }

    std::string version(Base64::encode(crypto_->getSHA1Hash(createByteArray(serializedCaps))));
    return CapsInfo(node_, version, "sha-1");
}

}
