/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/FileTransfer/FileTransferManager.h>

namespace Swift {

FileTransferManager::~FileTransferManager() {
}

bool FileTransferManager::isSupportedBy(const DiscoInfo::ref info) {
    if (info) {
        return info->hasFeature(DiscoInfo::JingleFeature)
                        && info->hasFeature(DiscoInfo::JingleFTFeature)
                        && (info->hasFeature(DiscoInfo::JingleTransportsIBBFeature) || info->hasFeature(DiscoInfo::JingleTransportsS5BFeature));
    }
    return false;
}

}
