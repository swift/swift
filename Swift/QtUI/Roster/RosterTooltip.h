/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QString>

#include <Swiften/Elements/VCard.h>

namespace Swift {

class ContactRosterItem;
class QtScaledAvatarCache;

class RosterTooltip {
    public:
        static QString buildDetailedTooltip(ContactRosterItem* contact, QtScaledAvatarCache* cachedImageScaler);

    private:
        static QString buildVCardSummary(VCard::ref vcard);
        static QString buildVCardField(bool preferred, const QString& name, const QString& content);
};

}
